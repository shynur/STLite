/*
    {V: vertex}   n=|V|
    {E: edge}     e=|E|
    (u, v): (un)directed edge, u:origin/tail -> v:destination/head
    (un)digraph, mixed graph
    e=(u, v), u and v are adjacent, uv and e are incident
    {PI: path}    m=|PI|
    directed acyclic graph, DAG
    (simple) path/cycle
    Eulerian/Hamiltonian tour
    wt(e)   weighted graph = weighted network = network = G(V, E, wt())
*/
#include <iostream>
#ifndef MINI_GRAPH
#define MINI_GRAPH
#include "..\miniConts\miniVector.hpp"
#include "..\miniConts\miniStack.hpp"
#include "..\miniConts\miniQueue.hpp"
template <typename vertex_t, typename edge_t>
class miniGraph {
	protected:
		using Rank = long;
		const static Rank Rank_MAX = 1 + ((1 << sizeof(Rank) * 8 - 2) - 1) * 2;
		enum Vstate {
			Undiscovered, Discovered, Visited,
		};
		enum Emode {
			Undetermined, Tree, Cross, Forward, Backward,
		};
	protected:
		struct Vertex {
			vertex_t data;
			Vstate status;
			Rank indeg, outdeg,
				 d_time, f_time,
				 parent, priority;
			Vertex() = default;
			Vertex(const Vertex& another)
				: data(another.data), status(another.status),
				  indeg(another.indeg), outdeg(another.outdeg),
				  d_time(another.d_time), f_time(another.f_time),
				  parent(another.parent), priority(another.priority) {}
			virtual Vertex& operator=(const Vertex& another) {
				data = another.data;
				status = another.status;
				indeg = another.indeg, outdeg = another.outdeg,
				d_time = another.d_time, f_time = another.f_time,
				parent = another.parent, priority = another.priority;
				return *this;
			}
			virtual ~Vertex() = default;
			Vertex(const vertex_t& data)
				: data(data),
				  indeg(0), outdeg(0),
				  status(miniGraph::Undiscovered),
				  d_time(-1), f_time(-1), parent(-1),
				  priority(miniGraph::Rank_MAX) {}
		};
		struct Edge {
			edge_t data;
			Rank weight,
				 adj_v;  //only used in AdjList
			Emode mode;
			Edge() = default;
			Edge(const Edge& another)
				: data(another.data), weight(another.weight),
				  adj_v(another.adj_v),
				  mode(another.mode) {}
			virtual Edge& operator=(const Edge& another) {
				data = another.data;
				weight = another.weight, adj_v = another.adj_v;
				mode = another.mode;
				return *this;
			}
			virtual ~Edge() = default;
			Edge(const edge_t& data, const Rank& weight = 0,
				 const Rank& adj_v = 0)
				: data(data), adj_v(adj_v), weight(weight),
				  mode(miniGraph::Undetermined) {}
		};
	protected:
		Rank reset(Rank e = 0) const {
			for (Rank u = 0; u < abs_V; ++u) {
				status(u) = Undiscovered;
				parent(u) = d_time(u) = f_time(u) = -1;
				priority(u) = Rank_MAX;

				for (Rank v = 0; v < abs_V; ++v)
					if (exist(u, v)) {
						mode(u, v) = Undetermined;
						++e;
					}
			}

			return e;
		}
		Rank BFS(Rank u, Rank& clk, const bool& visible = false) const {
			struct {
				const miniGraph *pgraph;
				miniQueue<miniVector<Rank>> bfs_tree;
				Rank operator<<(const Rank& u) {
					if (miniVector<Rank>& vec = bfs_tree.back();
							(!bfs_tree.empty())
							&& vec.find(pgraph->parent(u)) == -1)
						vec.insert(u);
					else
						bfs_tree.push(miniVector<Rank>(1, u));

					return u;
				}
				void operator()(const bool& visible = false) {
					if (visible) {
						for (; !bfs_tree.empty(); bfs_tree.pop()) {
							miniVector<Rank>& vec = bfs_tree.front();

							for (Rank i = 0, n = vec.size(); i < n; ++i)
								std::cout << pgraph->vertex(vec[i]) << "    ";

							std::cout << '\n';
						}

						std::cout << "\nPress any key "
								  << "to show the next BFS tree...\n";
						getchar();
					} else
						return;
				}
			} shower {this};
			Rank num_node = 0;
			status(u) = Discovered;

			for (miniQueue<Rank> aux_que(u);
					!aux_que.empty(); status(shower << u) = Visited) {
				d_time(u = aux_que.pop()) = ++clk;
				++num_node;

				for (Rank v = firstNbr(u); v >= 0; v = nextNbr(u, v))
					if (status(v) == Undiscovered) {
						status(v) = Discovered;
						aux_que.push(v);
						mode(u, v) = Tree;
						parent(v) = u;
					} else
						mode(u, v) = Cross;
			}

			shower(visible);
			return num_node;
		}
		Rank DFS(Rank u, Rank& clk,
				 const bool& new_time = false,
				 const bool& visible = false,
				 const bool& use_wchar = false) const {
			struct {
				const miniGraph *pgraph;
				const bool visible, use_wchar;
				Rank operator<<(const Rank& u) const {
					if (visible)
						if (use_wchar)
							std::wcout << L"->" << pgraph->vertex(u);
						else
							std::cout << "->" << pgraph->vertex(u);

					return u;
				}
				void new_ln(const Rank& depth = 0) const {
					if (visible && depth == 0)
						if (use_wchar)
							std::wcout << L'\n';
						else
							std::cout << '\n';
				}
				void new_depth(Rank node_depth) const {
					while (visible && node_depth-- != 0)
						if (use_wchar)
							std::wcout << L"    ";
						else
							std::cout << "   ";
				}
			} shower {this, visible, use_wchar};
			static Rank node_deepest_path, node_cur_depth;

			if (new_time)
				node_deepest_path = node_cur_depth = 0;

			node_deepest_path
				= node_deepest_path < ++node_cur_depth ?
				  node_cur_depth : node_deepest_path;
			d_time(u) = ++clk;
			status(shower << u) = Discovered;

			for (Rank v = firstNbr(u), next_time = 0; v >= 0; v = nextNbr(u, v))
				switch (status(v)) {
					case Undiscovered:
						mode(u, v) = Tree;
						parent(v) = u;

						if (next_time++ != 0)
							shower.new_ln(), shower.new_depth(node_cur_depth);

						DFS(v, clk, false, visible, use_wchar);
						break;

					case Discovered:
						mode(u, v) = Backward;
						break;

					case Visited:
						mode(u, v) = (d_time(v) > d_time(u) ? Forward : Cross);
						break;
				}

			status(u) = Visited;
			f_time(u) = ++clk;
			shower.new_ln(--node_cur_depth);
			return node_deepest_path;
		}
		miniStack<Rank>& BCC(Rank u, Rank& clk,
							 miniStack<Rank>& aux_stack,
							 miniVector<Rank>& ans_vec) const {
			auto hca = f_time;
			// highest connected ancestor
			(this->*hca)(u) = d_time(u) = ++clk, status(u) = Discovered;
			aux_stack.push(u);

			for (Rank v = firstNbr(u); v >= 0; v = nextNbr(u, v))
				switch (status(v)) {
					case Undiscovered:
						parent(v) = u;
						mode(u, v) = Tree;
						BCC(v, clk, aux_stack, ans_vec);

						if ((this->*hca)(v) < d_time(u))
							(this->*hca)(u) =
								(this->*hca)(u) <= (this->*hca)(v) ?
								(this->*hca)(u) : (this->*hca)(v);
						else
							for (outdeg(u) > 1 && ans_vec.insert(u);
									v != aux_stack.pop(); )
								;

						break;

					case Discovered:
						mode(u, v) = Backward;

						if (parent(u) != v)
							// find parent's proper ancestor
							(this->*hca)(u) = (this->*hca)(u) <= d_time(v) ?
											  (this->*hca)(u) : d_time(v);

						break;

					case Visited:
						mode(u, v) = d_time(v) > d_time(u) ? Forward : Cross;
						break;
				}

			status(u) = Visited;
			return aux_stack;
		}
		bool TopoSort(Rank u, Rank& clk, miniVector<vertex_t>& ans_vec) const {
			d_time(u) = ++clk, status(u) = Discovered;

			for (Rank v = firstNbr(u); v >= 0; v = nextNbr(u, v))
				switch (status(v)) {
					case Undiscovered:
						parent(v) = u, mode(u, v) = Tree;

						if (TopoSort(v, clk, ans_vec) == false)
							return false;

						break;

					case Discovered:
						mode(u, v) = Backward;
						return false;
						break;

					case Visited:
						mode(u, v) = (d_time(u) < d_time(v)) ? Forward : Cross;
						break;
				}

			status(u) = Visited;
			ans_vec.insert(vertex(u));
			return true;
		}
		template<typename PU> PU& PFS(Rank u, Rank& clk,
									  PU& priorUpdater) const {
			for (d_time(u) = ++clk,
					parent(u) = -1, priority(u) = 0, status(u) = Visited;
					true;
					status(u) = Visited, mode(parent(u), u) = Tree,
					d_time(u) = ++clk) {
				for (Rank v = firstNbr(u); v >= 0; v = nextNbr(u, v))
					priorUpdater(this, u, v);

				for (Rank cur_prior = Rank_MAX, v = 0; v < abs_V; ++v)
					if (status(v) == Undiscovered && priority(v) < cur_prior)
						cur_prior = priority(u = v);

				if (status(u) == Visited)
					break;
				else
					continue;
			}

			return priorUpdater;
		}
	public:
		miniGraph(): abs_V(0), abs_E(0) {}
		miniGraph(const miniGraph& another): abs_V(0), abs_E(0) {
			std::cerr << "\aShynur::miniConts::miniGraph cannot be copied!\n"
					  << "\aAlready invoked default-constructor for it.\n";
		}
		virtual miniGraph& operator=(const miniGraph& another) {
			std::cerr << "\aShynur::miniConts::miniGraph cannot be assigned!\n"
					  << "\aFunction's body of op.= is almost empty.\n";
			return *this;
		}
		virtual ~miniGraph() = default;
	protected:    // vertex
		Rank abs_V;
	public:
		Rank& get_n(const bool& if_change = false) {
			static Rank n;

			if (n = abs_V; if_change)
				return abs_V;
			else
				return n;
		}
		virtual vertex_t& vertex(const Rank& u = 0) const = 0;
		virtual Rank indeg(const Rank& u = 0) const = 0;
		virtual Rank outdeg(const Rank& u = 0) const = 0;
		virtual Rank firstNbr(const Rank& u = 0) const = 0;
		virtual Rank nextNbr(const Rank& u = 0, Rank v = 0) const = 0;
		virtual Vstate& status(const Rank& u = 0) const = 0;
		virtual Rank& d_time(const Rank& u = 0) const = 0;
		virtual Rank& f_time(const Rank& u = 0) const = 0;
		virtual Rank& parent(const Rank& u = 0) const = 0;
		virtual Rank& priority(const Rank& u = 0) const = 0;
		virtual Rank insert(const vertex_t& vertex) = 0;
		virtual vertex_t remove(const Rank& u = 0) = 0;
	protected:     // edge
		Rank abs_E;
	public:
		Rank& get_e(const bool& if_change = false) {
			static Rank e;

			if (e = abs_E; if_change)
				return abs_E;
			else
				return e;
		}
		virtual bool exist(const Rank& u = 0, const Rank& v = 0) const = 0;
		virtual Emode& mode(const Rank& u = 0, const Rank& v = 0) const = 0;
		virtual edge_t& edge(const Rank& u = 0, const Rank& v = 0) const = 0;
		virtual Rank& wt(const Rank& u = 0, const Rank& v = 0) const = 0;
		virtual bool bond(const edge_t& edge,
						  const Rank& u = 0, const Rank& v = 0,
						  const Rank& weight = 0) = 0;
		virtual edge_t cut(const Rank& u = 0, const Rank& v = 0) = 0;
		Rank undigraphy(Rank det_e = 0) {
			for (Rank u = 0; u < abs_V; ++u)
				for (Rank v = 0; v < abs_V; ++v)
					if (exist(u, v) && !exist(v, u)) {
						bond(edge(u, v), v, u, wt(u, v));
						++abs_E, ++det_e;
					}

			return det_e;
		}
	public:     // alg
		Rank bfs(const Rank& u = 0,
				 const bool& visible = false, Rank max_num_node = 0) const {
			reset();
			Rank clk = 0, v = u;

			do {
				if (Rank cur_num_node; status(v) == Undiscovered)
					if (max_num_node < (cur_num_node = BFS(v, clk, visible)))
						max_num_node = cur_num_node;

				++v;
			} while ((v %= abs_V) != u);

			return max_num_node;
		}
		Rank dfs(const Rank& u = 0,
				 const bool& visible = false, const bool& use_wchar = false,
				 Rank node_longest_path = 0) const {
			reset();
			Rank clk = 0, v = u;

			if (use_wchar)
				_setmode(_fileno(stdout), 0x10000);

			do {
				if (Rank cur_node_path; status(v) == Undiscovered)
					if (cur_node_path = DFS(v, clk, true, visible, use_wchar);
							node_longest_path < cur_node_path)
						node_longest_path = cur_node_path;

				++v;
			} while ((v %= abs_V) != u);

			if (use_wchar)
				_setmode(_fileno(stdout), 0x4000);

			return node_longest_path;
		}
		miniVector<Rank> *bcc(const Rank& u = 0,
							  Rank *const& pans = nullptr) const {
			reset();
			Rank v = u, clk = (pans == nullptr ? 0 : *pans = 0);
			miniVector<Rank> *ans_pvec = new miniVector<Rank>;
			miniStack<Rank> aux_stack;

			do {
				if (Rank pre_siz = ans_pvec->size();
						status(v) == Undiscovered) {
					BCC(v, clk, aux_stack, *ans_pvec).pop();

					if (Rank now_siz = ans_pvec->size(); pre_siz != now_siz)
						ans_pvec->remove(--now_siz);

					pans == nullptr ? 0 : ++(*pans);
				}

				++v;
			} while ((v %= abs_V) != u);

			ans_pvec->dedup();
			return ans_pvec;
		}
		miniVector<vertex_t> *tpSort(const Rank& u = 0,
									 bool *const& pexist = nullptr) const {
			Rank clk = 0, v = u;
			reset();
			miniVector<vertex_t> *ans_pvec = new miniVector<vertex_t>;
			pexist == nullptr ? false : *pexist = true;

			do {
				if (status(v) == Undiscovered)
					if (TopoSort(v, clk, *ans_pvec) == false) {
						while (!ans_pvec->empty())
							ans_pvec->remove(ans_pvec->size() - 1);

						pexist == nullptr ? false : *pexist = false;
						break;
					}

				++v;
			} while ((v %= abs_V) != u);

			return ans_pvec;
		}
		miniVector<miniVector<Rank>> *prim(const Rank& u = 0) const {
			miniVector<miniVector<Rank>> *msts
									  = new miniVector<miniVector<Rank>>;
			miniVector<Rank> mst_leaves;
			struct {
				miniVector<Rank>& vec;
				void operator()(const miniGraph<vertex_t, edge_t> *pgraph,
								const Rank& u, const Rank& v) const {
					if (pgraph->status(v) == Undiscovered)
						if (pgraph->priority(v) > pgraph->wt(u, v)) {
							pgraph->priority(v) = pgraph->wt(u, v);
							pgraph->parent(v) = u;
						}
				}
				void insert(const Rank& u, const Rank& v) {
					vec.insert(u),
							   vec.insert(v);
				}
			} PrimPU {mst_leaves};
			undigraphy();
			pfs(PrimPU, u);

			for (Rank u = 0; u < --get_n(); ++u)
				for (Rank v = u + 1; v < get_n(); ++v)
					if (exist(u, v))
						if (mode(u, v) == Tree || mode(v, u) == Tree)
							PrimPU.insert(u, v);

			for (Rank oldsiz = msts->size(); !mst_leaves.empty(); ) {
				oldsiz = msts->size();
				msts->insert(miniVector<Rank>(2));
				(*msts)[oldsiz][0] = mst_leaves.remove(mst_leaves.size() - 1);
				(*msts)[oldsiz][1] = mst_leaves.remove(mst_leaves.size() - 1);

				for (bool edge_exist = true; edge_exist; ) {
					edge_exist = false;

					for (Rank i = mst_leaves.size() - 2; i >= 0; i -= 2) {
						if ((*msts)[oldsiz].find(mst_leaves[i])
								+ (*msts)[oldsiz].find(mst_leaves[i + 1]) == -2)
							continue;
						else {
							edge_exist = true;
							(*msts)[oldsiz].insert(mst_leaves.remove(i)),
							(*msts)[oldsiz].insert(mst_leaves.remove(i));
						}
					}
				}
			}

			return msts;
		}
		miniVector<miniVector<Rank>> *dijkstra(const Rank& u = 0) const {
			miniVector<miniVector<Rank>> *spt =
										  new miniVector<miniVector<Rank>>;
			struct {
				miniVector<miniVector<Rank>>& spt;
				const miniGraph<vertex_t, edge_t>& G;
				miniVector<Rank> spt_branch;
				void operator()(const miniGraph<vertex_t, edge_t> *pG,
								const Rank& u, const Rank& v) const {
					if (pG->status(v) == Undiscovered)
						if (pG->priority(v) > pG->priority(u) + pG->wt(u, v)) {
							pG->priority(v) = pG->priority(u) + pG->wt(u, v);
							pG->parent(v) = u;
						}
				}
				void operator<<(const Rank& u) {
					spt_branch.insert(u);

					for (Rank v = G.firstNbr(u); v >= 0; v = G.nextNbr(u, v))
						if (G.mode(u, v) == Tree)
							(*this) << v;
				}
				void make_path(Rank v) {
					for (spt.insert(miniVector<Rank>()); v >= 0; v = G.parent(v))
						spt[spt.size() - 1].insert(v);

					spt[spt.size() - 1].reverse();
				}
			} DijkstraPU {*spt, *this};
			pfs(DijkstraPU, u);
			DijkstraPU << u;

			for (Rank i = 0; i < DijkstraPU.spt_branch.size(); ++i)
				DijkstraPU.make_path((DijkstraPU.spt_branch)[i]);

			return spt;
		}
		template<typename PU> PU& pfs(PU& priorUpdater,
									  const Rank& u = 0) const {
			reset();
			Rank clk = 0, v = u;

			do {
				if (status(v) == Undiscovered)
					PFS<PU>(v, clk, priorUpdater);

				++v;
			} while ((v %= abs_V) != u);

			return priorUpdater;
		}
};
#endif
