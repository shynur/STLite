#ifndef MINI_BALANCEDBST
#define MINI_BALANCEDBST
#include <iostream>
#include "..\miniConts\BSTree.hpp"
template<typename T> class SplayTree: public BSTree<T> {
		using Base = BSTree<T>;
	protected:
		using typename Base::Node;
		using Base::_size;
		using Base::_root;
		using Base::_hot;
		using Base::pick;
		using Base::updateHeight;
		using Base::updateHeightAbove;
	public:
		Node *&search(const T& elem, const bool& ref = false) {
			if (static Node *pnode = this->Base::search(elem); ref)
				return _root = splay(pnode == nullptr ? _hot : pnode);
			else
				return pnode = _root = splay(pnode == nullptr ? _hot : pnode);
		}
		Node *insert(const T& elem) {
			if (++_size; _root == nullptr)
				return _root = new Node(elem);
			else
				if (--_size; search(elem)->data == elem)
					return _root;
				else
					if (++_size; _root->data < elem) {
						_root = _hot->parent = new Node(elem, nullptr, _hot, _hot->rc);

						if (_hot->hasRightChild(_hot))
							_hot->rc->parent = _root, _hot->rc = nullptr;

						updateHeightAbove(_hot);
						return _root;
					} else {
						_root = _hot->parent = new Node(elem, nullptr, _hot->lc, _hot);

						if (_hot->hasLeftChild(_hot))
							_hot->lc->parent = _root, _hot->lc = nullptr;

						updateHeightAbove(_hot);
						return _root;
					}
		}
		bool remove(const T& elem);
	protected:
		static Node *splay(Node *v, Node *p = nullptr, Node *g = nullptr, Node *gg = nullptr) {
			if (v == nullptr)
				return nullptr;
			else {
				while ((p = v->parent) != nullptr && (g = p->parent) != nullptr) {
					if (gg = g->parent; v->isLeftChild(v))
						if (p->isLeftChild(p))
							adoptLeft(g, p->rc), adoptLeft(p, v->rc),
									  adoptRight(p, g), adoptRight(v, p);
						else
							adoptLeft(p, v->rc), adoptRight(g, v->lc),
									  adoptLeft(v, g), adoptRight(v, p);
					else
						if (p->isRightChild(p))
							adoptRight(g, p->lc), adoptRight(p, v->lc),
									   adoptLeft(p, g), adoptLeft(v, p);
						else
							adoptRight(p, v->lc), adoptLeft(g, v->rc),
									   adoptRight(v, g), adoptLeft(v, p);

					if (gg == nullptr)
						v->parent = nullptr;
					else
						gg->lc == g ? adoptLeft(gg, v) : adoptRight(gg, v);

					updateHeight(g), updateHeight(p), updateHeight(v);
				}

				if (p != nullptr) {
					if (v->isLeftChild(v))
						adoptLeft(p, v->rc), adoptRight(v, p);
					else
						adoptRight(p, v->lc), adoptLeft(v, p);

					updateHeight(p), updateHeight(v);
				}

				v->parent = nullptr;
				return v;
			}
		}
		static void adoptLeft(Node *const& p, Node *const& lc) {
			p->lc = lc == nullptr ? nullptr : lc->parent = p;
		}
		static void adoptRight(Node *const& p, Node *const& rc) {
			p->rc = rc == nullptr ? nullptr : rc->parent = p;
		}
};
template<typename T> class AVLTree: public BSTree<T> {
		using Base = BSTree<T>;
	protected:
		using typename Base::Node;
		using typename Base::Rank;
		using Base::_size;
		using Base::_hot;
		using Base::_root;
		using Base::pick;
		using Base::updateHeight;
		using Base::rot_at;
	public:
		using Base::search;
		Node *insert(const T& elem) {
			if (Node *&pnode = search(elem, true); pnode != nullptr)
				return pnode;
			else {
				Node *const pnew_node = pnode = new Node(elem, _hot);
				++_size;

				for (Node *ancestor = _hot; ancestor != nullptr; ancestor = ancestor->parent)
					if (Node **pparent_subtree; avl_balanced(ancestor))
						updateHeight(ancestor);
					else {
						Node *&parent_subtree = Node::fromParent(ancestor, this);
						parent_subtree = rot_at(taller_child(taller_child(ancestor)));
						break;
					}

				return pnew_node;
			}
		}
		bool remove(const T& elem) {
			if (Node *&to_pick = search(elem, true); to_pick == nullptr)
				return false;
			else {
				pick(to_pick, _hot);
				--_size;

				for (Node *ancestor = _hot; ancestor != nullptr; ancestor = ancestor->parent)
					if (avl_balanced(ancestor))
						updateHeight(ancestor);
					else {
						Node *&parent_subtree = Node::fromParent(ancestor, this);
						ancestor = parent_subtree = rot_at(taller_child(taller_child(ancestor)));
						updateHeight(ancestor);
					}

				return true;
			}
		}
	protected:
		static Rank balance_factor(const Node *const& pnode) {
			if (pnode == nullptr)
				return 0;
			else
				return Node::stature(pnode->lc) - Node::stature(pnode->rc);
		}
		static bool balanced(const Node *const& pnode) {
			return balance_factor(pnode) == 0;
		}
		static bool avl_balanced(const Node *const& pnode) {
			return -1 <= balance_factor(pnode) && balance_factor(pnode) <= 1;
		}
		static Node *&taller_child(Node *const& pnode) {
			if (balanced(pnode))
				return Node::isLeftChild(pnode) ? pnode->lc : pnode->rc;
			else
				if  (balance_factor(pnode) >= 1)
					return pnode->lc;
				else
					return pnode->rc;
		}
};
#endif
