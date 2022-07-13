#include <iostream>
#include "..\miniConts\miniGraph.hpp"
#ifndef MINI_ADJMATRIX
#define MINI_ADJMATRIX
template <typename vertex_t, typename edge_t>
class AdjMatrix: protected miniGraph<vertex_t, edge_t> {
	private:
		using Base = miniGraph<vertex_t, edge_t>;
	protected:
		using typename miniGraph<vertex_t, edge_t>::Rank;
		using miniGraph<vertex_t, edge_t>::Rank_MAX;
		using typename miniGraph<vertex_t, edge_t>::Vstate;
		using typename miniGraph<vertex_t, edge_t>::Emode;
		using typename miniGraph<vertex_t, edge_t>::Vertex;
		using typename miniGraph<vertex_t, edge_t>::Edge;
	protected:
		miniVector<Vertex> V;
		miniVector<miniVector<Edge *>> E;
	public:
		using miniGraph<vertex_t, edge_t>::get_n;
		using miniGraph<vertex_t, edge_t>::get_e;
	public:
		AdjMatrix() = default;
		AdjMatrix(const AdjMatrix& another) {
			std::cerr << "\aShynur::miniConts::miniGraph::AdjMatrix "
					  << "cannot be copied!\n";
		}
		AdjMatrix& operator=(const AdjMatrix& another) {
			std::cerr << "\aShynur::miniConts::miniGraph::AdjMatrix "
					  << "cannot be assigned!\n"
					  << "\aFunction's body of op.= is almost empty.\n";
			return *this;
		}
		~AdjMatrix() {
			for (Rank u = 0, n = get_n(); u < n; ++u)
				for (Rank v = 0; v < n; ++v)
					if (E[u][v] != nullptr)
						delete E[u][v];
		}
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
			for (Rank u = 0, n = get_n(); u < n; ++u)
				E[u].insert(nullptr);

			++get_n(true);
			E.insert(miniVector<Edge *>(get_n(), nullptr));
			return V.insert(Vertex(vertex));
		}
		vertex_t remove(const Rank& u = 0) override {
			for (Rank v = 0, n = get_n(); v < n; ++v)
				if (E[u][v] != nullptr) {
					delete E[u][v];
					--get_e(true), --V[v].indeg;
				}

			E.remove(u);
			--get_n(true);
			vertex_t to_remove = vertex(u);
			V.remove(u);

			for (Rank u0 = 0, n = get_n(); u0 < n; ++u0)
				if (Edge *pedge = E[u0][u]; E[u0].remove(u) != nullptr) {
					delete pedge;
					--get_e(true),
					--V[u0].outdeg;
				}

			return to_remove;
		}
	public:
		bool exist(const Rank& u = 0, const Rank& v = 0) const override {
			return E[u][v] != nullptr
				   && (0 <= u && u < get_n())
				   && (0 <= v && v < get_n());
		}
		Emode& mode(const Rank& u = 0, const Rank& v = 0) const override {
			return E[u][v]->mode;
		}
		edge_t& edge(const Rank& u = 0, const Rank& v = 0) const override {
			return E[u][v]->data;
		}
		Rank& wt(const Rank& u = 0, const Rank& v = 0) const override {
			return E[u][v]->weight;
		}
		bool bond(const edge_t& edge,
				  const Rank& u = 0, const Rank& v = 0,
				  const Rank& weight = 0) override {
			if (exist(u, v))
				return false;
			else {
				E[u][v] = new Edge(edge, weight);
				++get_e(true),
				++V[u].outdeg, ++V[v].indeg;
				return true;
			}
		}
		edge_t cut(const Rank& u = 0, const Rank& v = 0) override {
			edge_t to_remove = edge(u, v);
			delete E[u][v];
			E[u][v] = nullptr;
			--get_e(true),
			--V[u].outdeg, --V[v].indeg;
			return to_remove;
		}
};
#endif
