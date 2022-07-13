#include <iostream>
#include "..\miniConts\miniStack.hpp"
#include "..\miniConts\miniQueue.hpp"
#ifndef MINI_BINTREE
#define MINI_BINTREE
template <typename T>
class BinTree {
	protected:
		using Rank = long long;
		enum Color {RED, BLACK,};
		struct Node;
		static Rank Rand() {
			return std::rand();
		}
	protected:
		Rank _size;
		Node *_root;
	protected:
		struct Node {
			protected:
				Rank Rand() const {
					return BinTree::Rand();
				}
			public:
				T data;
				Node *parent, *lc, *rc;
				Rank height;
				Rank npl;
				Color color;
				Node():
					parent(nullptr), lc(nullptr), rc(nullptr),
					height(0), npl(1), color(RED) {}
				Node(const Node& another)
					: data(another.data),
					  parent(another.parent), lc(another.lc), rc(another.rc),
					  height(another.height),
					  npl(another.npl),
					  color(another.color) {}
				virtual Node& operator=(const Node& another) {
					data = another.data;
					parent = another.parent, lc = another.lc, rc = another.rc;
					height = another.height, npl = another.npl;
					color = another.color;
					return *this;
				}
				virtual ~Node() = default;
			public:
				Node(const T& e,
					 Node *const& parent = nullptr,
					 Node *const& lc = nullptr,
					 Node *const& rc = nullptr,
					 const Rank& height = 0, const Rank& npl = 1,
					 const Color& color = RED)
					: data(e), parent(parent), lc(lc), rc(rc),
					  height(height), npl(npl), color(color) {}
				Rank size(const Rank& choice = 0) {
					struct {
						Rank n = 0;
						void operator()(const T& e) {
							++n;
						}
					} functor;

					switch (choice == 0 ? 1 + Rand() % 9 : choice) {
						case 1:
							return travPre_i1(functor).n;

						case 2:
							return travPre_i2(functor).n;

						case 3:
							return travPre_r(functor).n;

						case 4:
							return travIn_i1(functor).n;

						case 5:
							return travIn_i2(functor).n;

						case 6:
							return travIn_r(functor).n;

						case 7:
							return travPost_i1(functor).n;

						case 8:
							return travPost_r(functor).n;

						case 9:
						default:
							return travLv(functor).n;
					}
				}
				Node *gen_lc(const T& e) {
					return lc = new Node(e, this);
				}
				Node *gen_rc(const T& e) {
					return rc = new Node(e, this);
				}
				Node *suc() {
					if (Node *pnode = this; rc != nullptr) {
						pnode = pnode->rc;

						while (pnode->hasLeftChild(pnode))
							pnode = pnode->lc;

						return pnode;
					} else {
						while (pnode->isRightChild(pnode))
							pnode = pnode->parent;

						pnode = pnode->parent;
						return pnode;
					}
				}

				template<typename obj> obj& travLv(obj& functor) {
					for (miniQueue<Node *> tmp(this); !tmp.empty(); ) {
						Node *pnode = tmp.pop();
						functor(pnode->data);

						if (pnode->hasLeftChild(pnode))
							tmp.push(pnode->lc);

						if (pnode->hasRightChild(pnode))
							tmp.push(pnode->rc);
					}

					return functor;
				}
				template<typename obj> obj& travPre(obj& functor,
													const Rank& choice = 0) {
					switch (choice == 0 ? 1 + Rand() % 3 : choice) {
						case 1:
							return travPre_i1(functor);

						case 2:
							return travPre_i2(functor);

						case 3:
						default:
							return travIn_r(functor);
					}
				}
				template<typename obj> obj& travPre_i1(obj& functor) {
					miniStack<Node *> tmp(this);

					for (Node * pnode; !tmp.empty();
							pnode->rc == nullptr ? 0 : tmp.push(pnode->rc),
							pnode->lc == nullptr ? 0 : tmp.push(pnode->lc))
						functor((pnode = tmp.pop())->data);

					return functor;
				}
				template<typename obj> obj& travPre_i2(obj& functor) {
					miniStack<Node *> tmp;

					for (Node *pnode = this; true; ) {
						for ( ; pnode != nullptr; pnode = pnode->lc) {
							functor(pnode->data);
							tmp.push(pnode->rc);
						}

						if (tmp.empty())
							break;
						else
							pnode = tmp.pop();
					}

					return functor;
				}
				template<typename obj> obj& travPre_r(obj& functor) {
					if (this == nullptr)
						return functor;
					else {
						functor(data);
						return rc->travPre_r(lc->travPre_r(functor));
					}
				}
				template<typename obj> obj& travIn(obj& functor,
												   const Rank& choice = 0) {
					switch (choice == 0 ? 1 + Rand() % 3 : choice) {
						case 1:
							return travIn_i1(functor);

						case 2:
							return travIn_i2(functor);

						case 3:
						default:
							return travIn_r(functor);
					}
				}
				template<typename obj> obj& travIn_i1(obj& functor) {
					miniStack<Node *> tmp;

					for (Node *pnode = this; true; pnode = pnode->rc) {
						for ( ; pnode != nullptr; pnode = pnode->lc)
							tmp.push(pnode);

						if (tmp.empty())
							break;
						else
							functor((pnode = tmp.pop())->data);
					}

					return functor;
				}
				template<typename obj> obj& travIn_i2(obj& functor) {
					Node *pnode = this;

					while (pnode->hasLeftChild(pnode))
						pnode = pnode->lc;

					do {
						functor(pnode->data);
						pnode = pnode->suc();
					} while (pnode != nullptr && pnode != this->parent);

					return functor;
				}
				template<typename obj> obj& travIn_r(obj& functor) {
					if (this == nullptr)
						return functor;
					else {
						lc->travIn_r(functor)(data);
						return rc->travIn_r(functor);
					}
				}
				template<typename obj> obj& travPost(obj& functor,
													 const Rank& choice = 0) {
					switch (choice == 0 ? 1 + Rand() % 2 : choice) {
						case 1:
							return travIn_i1(functor);

						case 2:
						default:
							return travIn_r(functor);
					}
				}
				template<typename obj> obj& travPost_i1(obj& functor) {
					miniStack<Node *> tmp(this);
					Node *pnode;

					for (pnode = this; !tmp.empty(); ) {
						for (; pnode->hasChildren(pnode); pnode = pnode->lc)
							tmp.push(pnode->rc);

						if (pnode->isLeaf(pnode)) {
							functor(pnode->data);

							while (pnode->sibling(pnode) != tmp.top())
								functor((pnode = pnode->parent)->data);

							pnode = tmp.pop();
						} else
							if (pnode->hasRightChild(pnode))
								pnode = pnode->rc;
							else
								pnode = pnode->lc;
					}

					return functor;
				}
				template<typename obj> obj& travPost_r(obj& functor) {
					if (this == nullptr)
						return functor;
					else {
						rc->travPost_r(lc->travPost_r(functor))(data);
						return functor;
					}
				}
				Rank wither(Rank n = 0) {
					n += (lc == nullptr ? 0 : lc->wither())
						 + (rc == nullptr ? 0 : rc->wither());
					delete this;
					return ++n;
				}
				bool operator<(const Node& another) const {
					return data < another.data;
				}
				bool operator==(const Node& another) const {
					return data == another.data;
				}
				bool operator<=(const Node& another) const {
					return *this < another || *this == another;
				}
				bool operator>=(const Node& another) const {
					return ! *this < another;
				}
				bool operator>(const Node& another) const {
					return !*this <= another;
				}
				bool operator!=(const Node& another) const {
					return !*this == another;
				}
				static Rank stature(const Node *const& pnode) {
					if (pnode != nullptr)
						return pnode->height;
					else
						return -1;
				}
				static bool isRoot(const Node *const& pnode) {
					return pnode->parent == nullptr;
				}
				static bool isLeftChild(const Node *const& pnode) {
					return isRoot(pnode) == false
						   && pnode->parent->lc == pnode;
				}
				static bool isRightChild(const Node *const& pnode) {
					return isRoot(pnode) == false
						   && pnode->parent->rc == pnode;
				}
				static bool hasParent(const Node *const& pnode) {
					return isLeftChild(pnode)
						   || isRightChild(pnode);
				}
				static bool hasLeftChild(const Node *const& pnode) {
					return pnode->lc != nullptr;
				}
				static bool hasRightChild(const Node *const& pnode) {
					return pnode->rc != nullptr;
				}
				static bool hasChild(const Node *const& pnode) {
					return hasLeftChild(pnode)
						   || hasRightChild(pnode);
				}
				static bool hasOnlyChild(const Node *const& pnode) {
					return hasChild(pnode)
						   && !hasChildren(pnode);
				}
				static bool hasChildren(const Node *const& pnode) {
					return hasLeftChild(pnode)
						   && hasRightChild(pnode);
				}
				static bool isLeaf(const Node *const& pnode) {
					return hasChild(pnode) == false;
				}
				static Node *&sibling(Node *&pnode) {
					if (isRoot(pnode))
						return pnode;
					else
						if (isLeftChild(pnode))
							return pnode->parent->rc;
						else
							return pnode->parent->lc;
				}
				static Node *&aunt(const Node *const& pnode) {
					if (const Node * const& pnode_parent = pnode->parent;
							isChild(pnode_parent))
						return sibling(pnode_parent);
					else
						return nullptr;
				}
				static Node *&fromParent(Node *&pnode, BinTree *tree = nullptr) {
					if (isRoot(pnode))
						return tree == nullptr ? pnode : tree->_root;
					else
						if (isLeftChild(pnode))
							return pnode->parent->lc;
						else
							return pnode->parent->rc;
				}
		};
	protected:
		static Rank updateHeight(Node *const& pnode) {
			if (Rank lc_height = pnode->stature(pnode->lc),
					rc_height = pnode->stature(pnode->rc);
					lc_height >= rc_height)
				return pnode->height = lc_height + 1;
			else
				return pnode->height = rc_height + 1;
		}
		static Rank updateHeightAbove(Node *pnode, Rank tmp = 0) {
			if (pnode != nullptr)
				for (tmp = updateHeight(pnode); pnode != nullptr; pnode = pnode->parent)
					updateHeight(pnode);

			return tmp;
		}
	public:
		BinTree(Node *const& root = nullptr,
				const Rank& size = 0): _size(size), _root(root) {}
		BinTree(const BinTree& another)
			: _size(0), _root(nullptr) {
			std::cerr << "\aShynur::miniConts::BinTree cannot be copied!\n"
					  << "\aAlready invoked default-constructor for it.\n";
		}
		virtual BinTree& operator=(const BinTree& another) {
			std::cerr
					<< "\aShynur::miniConts::BinTree cannot be assigned!\n"
					<< "\aFunction's body of op.= is almost empty.\n"
					<< "\aMaybe its Derived can. Please check.\n";
			return *this;
		}
		virtual ~BinTree() {
			if (_size >= 1)
				_root->wither();
		}
	public:
		Rank size(Rank *const& pn = nullptr) const {
			if (pn == nullptr)
				return _size;
			else
				return *pn = _size;
		}
		bool empty(bool *const& pbool = nullptr) const {
			if (pbool == nullptr)
				return _size <= 0;
			else
				return *pbool = (_size <= 0);
		}
		Node *root(Node *&pnode = nullptr) {
			if (pnode == nullptr)
				return _root;
			else
				return pnode = _root;
		}
		Node *make_root(const T& e) {
			if (!empty())
				_root->wither();

			_size = 1;
			return _root = new Node(e);
		}
		Node *gen_lc(Node *const& pnode, const T& e) {
			++_size;
			pnode->gen_lc(e);
			updateHeightAbove(pnode);
			return pnode->lc;
		}
		Node *gen_rc(Node *const& pnode, const T& e) {
			++_size;
			pnode->gen_rc(e);
			updateHeightAbove(pnode);
			return pnode->rc;
		}
		Node *gen_lt(Node *const& pnode, BinTree& tree) {
			if ((pnode->lc = tree._root) != nullptr)
				pnode->lc->parent = pnode;

			_size += tree._size;
			updateHeightAbove(pnode);
			tree._size = 0;
			return pnode;
		}
		Node *gen_rt(Node *const& pnode, BinTree& tree) {
			if ((pnode->rc = tree._root) != nullptr)
				pnode->rc->parent = pnode;

			_size += tree._size;
			updateHeightAbove(pnode);
			tree._size = 0;
			return pnode;
		}

		BinTree *secede(Node *const& pnode) {
			if (pnode->hasParent(pnode))
				Node::fromParent(pnode) = nullptr;   // parent leave

			updateHeightAbove(pnode->parent);
			BinTree *ptree = new BinTree(pnode, pnode->size());
			pnode->parent = nullptr; // bye, parent
			_size -= ptree->_size;
			return ptree;
		}
		Rank wither(Node *&pnode, Rank n = 0) {
			if (pnode->hasParent(pnode))
				Node::fromParent(pnode) = nullptr;

			_size -= (n = pnode->wither());
			return n;
		}
		template<typename obj> obj& travLv(obj& functor) {
			if (_root == nullptr)
				return functor;
			else
				return _root->travLv(functor);
		}
		template<typename obj> obj& travPre(obj& functor,
											const Rank& choice = 0) {
			if (_root == nullptr)
				return functor;
			else
				return _root->travPre(functor, choice);
		}
		template<typename obj> obj& travIn(obj& functor,
										   const Rank& choice = 0) {
			if (_root == nullptr)
				return functor;
			else
				return _root->travIn(functor, choice);
		}
		template<typename obj> obj& travPost(obj& functor,
											 const Rank& choice = 0) {
			if (_root == nullptr)
				return functor;
			else
				return _root->travPost(functor, choice);
		}
		bool operator==(const BinTree& another) const {
			return _root == another._root;
		}
		bool operator!=(const BinTree& another) const {
			return !this->operator==(another);
		}
		bool operator<(const BinTree& another) const;
		bool operator<=(const BinTree& another) const;
		bool operator>=(const BinTree& another) const;
		bool operator>(const BinTree& another) const;
};
#endif
