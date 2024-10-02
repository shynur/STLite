#include <iostream>
#ifndef MINI_ADJLIST
#define MINI_ADJLIST
#include "..\miniConts\miniGraph.hpp"
template <typename vertex_t, typename edge_t>
class AdjList: protected miniGraph<vertex_t, edge_t> {
	private:
		using Base = miniGraph<vertex_t, edge_t>;
	protected:
		using typename Base::Rank;
		using Base::Rank_MAX;
		using typename Base::Vertex;
		using typename Base::Vstate;
		using typename Base::Edge;
		using typename Base::Emode;
	protected:
		miniVector<Vertex> V;
		miniVector<miniList<Edge>> E;
	public:
		using Base::get_n;
		using Base::get_e;
	public:
		AdjList() = default;
		AdjList(const AdjList& another) {
			std::cerr << "\aShynur::miniConts::miniGraph::AdjList "
					  << "cannot be copied!\n";
		}
		AdjList& operator=(const AdjList& another) {
			std::cerr << "\aShynur::miniConts::miniGraph::AdjMatrix "
					  << "cannot be assigned!\n"
					  << "\aFunction's body of op.= is almost empty.\n";
			return *this;
		}
		~AdjList() = default;
	public:
		using Base::bfs;
		using Base::dfs;
		using Base::bcc;
        using Base::prim;
        using Base::dijkstra;
		using Base::tpSort;
		using Base::undigraphy;
	public: // implement of base's virtual-function
		vertex_t& vertex(const Rank& u = 0) const override {
			return V[u].data;
		}
		Rank indeg(const Rank& u = 0) const override {
			return V[u].indeg;
		}
		Rank outdeg(const Rank& u = 0) const override {
			return V[u].outdeg;
		}
		Rank firstNbr(const Rank& u = 0) const override {
			if (E[u].empty())
				return -1;
			else
				return nextNbr(u, get_n());
		}
		Rank nextNbr(const Rank& u = 0, Rank v = 0) const override {
			while (--v >= 0)
				if (exist(u, v))
					break;

			return v;
		}
		Vstate& status(const Rank& u = 0) const override {
			return V[u].status;
		}
		Rank& d_time(const Rank& u = 0) const override {
			return V[u].d_time;
		}
		Rank& f_time(const Rank& u = 0) const override {
			return V[u].f_time;
		}
		Rank& parent(const Rank& u = 0) const override {
			return V[u].parent;
		}
		Rank& priority(const Rank& u = 0) const override {
			return V[u].priority;
		}
		Rank insert(const vertex_t& vertex) override {
			++get_n(true);
			E.insert(miniList<Edge>());
			return V.insert(Vertex(vertex));
		}
		vertex_t remove(const Rank& u = 0) override {
			for (; !E[u].empty(); --get_e(true))
				--V[(E[u].remove(E[u].first())).adj_v].indeg;

			E.remove(u);
			--get_n(true);
			vertex_t to_remove = vertex(u);
			V.remove(u);

			for (Rank u0 = 0, n = get_n(); u0 < n; ++u0)
				if (auto pedge = E[u0].first(); exist(u0, u)) {
					// here typename auto
					// = miniList<miniGraph<vertex_t, edge_t>::Edge>::node *
					while ((pedge->data).adj_v != u)
						pedge = pedge->suc;

					E[u0].remove(pedge);
					--get_e(true),
					--V[u0].outdeg;
				}

			return to_remove;
		}
	public:
		bool exist(const Rank& u = 0, const Rank& v = 0) const override {
			struct {
				Rank v;
				bool if_exist;
				void operator()(const Edge& edge_from_u) {
					if (if_exist)
						return;
					else
						if (edge_from_u.adj_v == v)
							if_exist = true;
						else
							return;
				}
			} functor {v, false};
			return (0 <= u && u < get_n()) && (0 <= v && v < get_n())
				   && (E[u].traverse(functor)).if_exist;
		}
		Emode& mode(const Rank& u = 0, const Rank& v = 0) const override {
			auto pedge = E[u].first();

			while ((pedge->data).adj_v != v)
				pedge = pedge->suc;

			return (pedge->data).mode;
		}
		edge_t& edge(const Rank& u = 0, const Rank& v = 0) const override {
			auto pedge = E[u].first();

			while ((pedge->data).adj_v != v)
				pedge = pedge->suc;

			return (pedge->data).data;
		}
		Rank& wt(const Rank& u = 0, const Rank& v = 0) const override {
			auto pedge = E[u].first();

			while ((pedge->data).adj_v != v)
				pedge = pedge->suc;

			return (pedge->data).weight;
		}
		bool bond(const edge_t& edge,
				  const Rank& u = 0, const Rank& v = 0,
				  const Rank& weight = 0) override {
			if (exist(u, v))
				return false;
			else {
				E[u].makelast(Edge(edge, weight, v));
				++get_e(true),
				++V[u].outdeg, ++V[v].indeg;
				return true;
			}
		}
		edge_t cut(const Rank& u = 0, const Rank& v = 0) override {
			edge_t to_remove = edge(u, v);
			{
				auto pedge = E[u].first();

				while ((pedge->data).adj_v != v)
					pedge = pedge->suc;

				E[u].remove(pedge);
			}
			--get_e(true),
			--V[u].outdeg, --V[v].indeg;
			return to_remove;
		}
};
#endif
