#ifndef MINI_BSTREE
#define MINI_BSTREE
#include <iostream>
#include "..\miniConts\BinTree.hpp"
template<typename K, typename V> struct Entry {
		K key;
		V val;
		Entry(const K& key = K(), const V& val = V())
			: key(key), val(val) {}
		Entry(const Entry& another): key(another.key), val(another.val) {}
		Entry& operator=(const Entry& another) {
			key = another.key;
			val = another.val;
			return *this;
		}
		~Entry() = default;
	public:
		bool operator<(const Entry& another) const {
			return key < another.key;
		}
		bool operator==(const Entry& another) const {
			return key == another.key;
		}
		bool operator<=(const Entry& another) const {
			return *this < another || *this == another;
		}
		bool operator!=(const Entry& another) const {
			return ! *this == another;
		}
		bool operator>=(const Entry& another) const {
			return ! *this < another;
		}
		bool operator>(const Entry& another) const {
			return ! *this <= another;
		}
};
template<typename T> class BSTree: public BinTree<T> {
		using Base = BinTree<T>;
	protected:
		using typename Base::Rank;
		using typename Base::Node;
		using Base::_root;
		using Base::_size;
		using Base::updateHeight;
		using Base::updateHeightAbove;
	protected:
		Node *_hot;
		Node *recombine_3node4tree(Node *W, Node *U, Node *V,
								   Node *u1, Node *u2, Node *v1, Node *v2) const {
			(U->lc = u1) == nullptr ? nullptr : u1->parent = U,
			(U->rc = u2) == nullptr ? nullptr : u2->parent = U,
			(V->lc = v1) == nullptr ? nullptr : v1->parent = V,
			(V->rc = v2) == nullptr ? nullptr : v2->parent = V;
			updateHeight(U), updateHeight(V);
			updateHeight((W->lc = U)->parent = (W->rc = V)->parent = W);
			return W;
		}
		Node *rot_at(Node *v, Node *p = nullptr, Node *g = nullptr) const {
			if (g = (p = v->parent)->parent; Node::isLeftChild(p))
				if (Node::isLeftChild(v)) {
					p->parent = g->parent;
					return recombine_3node4tree(p, v, g, v->lc, v->rc, p->rc, g->rc);
				} else {
					v->parent = g->parent;
					return recombine_3node4tree(v, p, g, p->lc, v->lc, v->rc, g->rc);
				} else
				if (Node::isRightChild(v)) {
					p->parent = g->parent;
					return recombine_3node4tree(p, g, v, g->lc, p->lc, v->lc, v->rc);
				} else {
					v->parent = g->parent;
					return recombine_3node4tree(v, g, p, g->lc, v->lc, v->rc, p->rc);
				}
		}
	public:
		BSTree() = default;
		BSTree(const BSTree& another): Base(another), _hot(nullptr) {};
		BSTree& operator=(const BSTree& another) {
			_hot = another._hot;
			return Base::operator=(another);
		}
		~BSTree() = default;
	public:
		virtual Node *&search(const T& elem, const bool& ref = false) {
			if (static Node * pnode; ref)
				return searchIn(_root, elem, _hot = nullptr);
			else
				return pnode = searchIn(_root, elem, _hot = nullptr);
		}
		virtual Node *insert(const T& elem) {
			if (Node *&pnode = search(elem, true); pnode != nullptr)
				return pnode;
			else {
				pnode = new Node(elem, _hot);
				++_size;
				updateHeightAbove(pnode);
				return pnode;
			}
		}
		virtual bool remove(const T& elem) {
			if (Node *&to_pick = search(elem, true); to_pick == nullptr)
				return false;
			else {
				pick(to_pick, _hot);
				--_size;
				updateHeightAbove(_hot);
				return true;
			}
		}
	protected:
		static Node *&searchIn(Node *&fork, const T& elem,
							   Node *&hot) {
			if (fork == nullptr || fork->data == elem)
				return fork;
			else
				if (hot = fork; elem < fork->data)
					return searchIn(fork->lc, elem, hot);
				else
					return searchIn(fork->rc, elem, hot);
		}
		static Node *pick(Node *&to_pick, Node *&hot,
						  Node *picked = nullptr, Node *successor = nullptr) {
			if (T tmp; !to_pick->hasChildren(to_pick))
				if (hot = (picked = to_pick)->parent; picked->hasLeftChild(picked))
					successor = to_pick = picked->lc;
				else
					successor = to_pick = picked->rc;
			else {
				hot = (picked = to_pick->suc())->parent;
				tmp = to_pick->data,
				to_pick->data = picked->data,
						 picked->data = to_pick->data;
				return pick(Node::fromParent(picked), hot);
			}

			successor == nullptr ? nullptr : successor->parent = hot;
			picked->parent = picked->lc = picked->rc = nullptr;
			picked->wither();
			return successor;
		}
};
#endif
