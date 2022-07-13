#include <iostream>
#ifndef MINI_LIST
#define MINI_LIST
template <typename T>
class miniList {
	protected:
		typedef long Rank;
		struct node {
			T data;
			node *pre, *suc;
			node() = default;
			node(const node& another)
				: data(another.data), pre(another.pre), suc(another.suc) {}
			virtual node& operator=(const node& another) {
				data = another.data;
				pre = another.pre, suc = another.suc;
				return *this;
			}
			virtual ~node() = default;
			node(const T& data,
				 node *const& pre = nullptr,
				 node *const& suc = nullptr)
				: data(data), pre(pre), suc(suc) {}
			node *makepre(const T& elem) {
				return pre = pre->suc = new node(elem, pre, this);
			}
			node *makesuc(const T& elem) {
				return suc = suc->pre = new node(elem, this, suc);
			}
		};
		Rank Rand() const {
			return std::rand();
		}
	protected:
		Rank _size;
		node *head, *tail;
	protected:
		Rank clear() {
			Rank old_size = _size;

			while (!empty())
				remove(first());

			return old_size;
		}
		void cpy(node *p, Rank n) {
			// in fact, it means copy them to the tail
			while (n--) {
				makelast(p->data);
				p = p->suc;
			}
		}
		void insertionSort(node *p, const Rank& n) {
			for (Rank r = 0; r < n; ++r) {
				right(search(p->data, r, p), p->data);
				p = p->suc;
				remove(p->pre);
			}
		}
		void mergeSort(node *p, Rank n) {
			if (Rank m = n / 2; n <= 1)
				return;
			else {
				node *q = p, *cur = p->pre;

				for (Rank i = 0; i < m; ++i)
					q = q->suc;

				n = n - m;
				// now [p, p+m) + [q, q+n), and p+m==q
				mergeSort(p, m), mergeSort(q, n);
				p = cur->suc, q = p;                    // fuck!

				for (Rank i = 0; i < m; ++i)
					q = q->suc;                         // shit!

				// merging...
				while (m > 0 || n > 0)
					if (m > 0 && n > 0)
						if (p->data <= q->data) {
							p = p->suc;
							cur = right(cur, remove(p->pre));
							--m;
						} else {
							q = q->suc;
							cur = right(cur, remove(q->pre));
							--n;
						} else {
						while (m > 0)
							cur = right(cur, remove((p = p->suc)->pre)), --m;

						while (n > 0)
							cur = right(cur, remove((q = q->suc)->pre)), --n;
					}
			}
		}
		void selectionSort(node *p, Rank n) {
			node *q = p;

			for (Rank i = 0; i < n; ++i)
				q = q->suc;

			p = p->pre;
			// now (p, q) because p may be delete if [p, q)

			while (n)
				q = left(q, remove(max(p->suc, n--)));
		}
	public:
		/*--- constructors & destructors ---*/
		miniList()
			: _size(0), head(new node), tail(new node) {
			head->suc = tail, tail->pre = head;
		}
		miniList(const miniList& another)
			: _size(0), head(new node), tail(new node) {
			head->suc = tail, tail->pre = head;
			cpy(another.first(), another._size);
		}
		virtual miniList& operator=(const miniList& another) {
			clear();
			cpy(another.first(), another._size);
			return *this;
		}
		virtual ~miniList() {
			clear();
			delete head, delete tail;
		}
	public:
		miniList(const miniList& another, Rank r, const Rank& n)
			: _size(0), head(new node), tail(new node) {
			head->suc = tail, tail->pre = head;
			node *p = another.first();

			while (r--)
				p = p->suc;

			cpy(p, n);
		}
		miniList(node *const& p, const Rank& n)
			: _size(0), head(new node), tail(new node) {
			head->suc = tail, tail->pre = head;
			cpy(p, n);
		}
		/*--- getter ---*/
		Rank disordered() const;
		bool empty() const {
			return _size <= 0;
		}
		node *find(const T& elem, Rank n, node *p) const {
			if (p = p->pre; n == 0)
				return nullptr;
			else
				for (head->data = elem; n-- != 0; p = p->pre)
					if (p->data == elem)
						if (p != head)
							return p;
						else
							return nullptr;
		}
		node *find(const T& elem) const {
			return find(elem, _size, tail);
		}
		node *first() const {
			return head->suc;
		}
		node *last() const {
			return tail->pre;
		}
		node *max(node *p, Rank n) const {
			for (node *tmp = p; n-- != 0; tmp = tmp->suc)
				if (tmp->data > p->data)
					p = tmp;

			return p;
		}
		node *max() const {
			return max(head->suc, _size);
		}
		T& operator[](Rank r) const {
			if (node *p = first(); r < _size / 2) {
				while (r--)
					p = p->suc;

				return p->data;
			} else {
				p = last();

				while (++r != _size)
					p = p->pre;

				return p->data;
			}
		}
		Rank size() const {
			return _size;
		}
		node *search(const T& elem, Rank n, node *p) const {
			// search from {p-1, p-2, ..., p-n}
			for (head->data = elem, p = p->pre; n-- != 0; p = p->pre)
				if (p->data <= elem)
					return p;

			return p;    // return p=p-n-1 when false
		}
		node *search(const T& elem) const {
			return search(elem, _size, tail);
		}
		/*--- setter ---*/
		Rank dedup() {
			Rank old_size = _size, r = 0;

			for (node *p = first(); p != tail; )
				if (find(p->data, r, p) == nullptr) {
					p = p->suc;
					++r;
				} else {
					p = p->suc;
					remove(p->pre);
				}

			return old_size - _size;
		}
		node *left(node *const& p, const T& elem) {
			++_size;
			return p->makepre(elem);
		}
		node *makefirst(const T& elem) {
			return right(head, elem);
		}
		node *makelast(const T& elem) {
			return left(tail, elem);
		}
		node *right(node *const& p, const T& elem) {
			++_size;
			return p->makesuc(elem);
		}
		void reverse() const;
		T remove(node *const p) {
			// originally i put 'node *const& p' here, but it's a very deep bug!
			T tmp = p->data;
			(p->pre)->suc = p->suc;
			(p->suc)->pre = p->pre;
			delete p;
			--_size;
			return tmp;
		}
		void sort(node *const& p, const Rank& n,
				  const Rank& choice = 0) {
			switch (choice == 0 ? 1 + Rand() % 3 : choice) {
				case 1:
					insertionSort(p, n);
					break;

				case 2:
					selectionSort(p, n);
					break;

				case 3:
					mergeSort(p, n);
					break;
			}
		}
		void sort(const Rank& choice = 0) {
			sort(first(), _size, choice);
		}
		Rank show(Rank n = 0) const {
			if (n == 0)
				n = _size;

			node *p = first();

			for (Rank i = 0; i < n; p = p->suc)
				std::cout << i++ << '\t' << p->data << '\n';

			return n;
		}
		template<typename ret> void *traverse(ret func(T)) const {
			for (node *p = first(); p != tail; p = p->suc)
				func(p->data);

			return (void *)func;
		}
		template<typename ret> void *traverse(ret& func(T)) const {
			for (node *p = first(); p != tail; p = p->suc)
				func(p->data);

			return (void *)func;
		}
		template<typename ret> void *traverse(const ret& func(T)) const {
			for (node *p = first(); p != tail; p = p->suc)
				func(p->data);

			return (void *)func;
		}
		template<typename ret> void *traverse(ret func(T&)) const {
			for (node *p = first(); p != tail; p = p->suc)
				func(p->data);

			return (void *)func;
		}
		template<typename ret> void *traverse(ret func(const T&)) const {
			for (node *p = first(); p != tail; p = p->suc)
				func(p->data);

			return (void *)func;
		}
		template<typename ret> void *traverse(ret& func(T&)) const {
			for (node *p = first(); p != tail; p = p->suc)
				func(p->data);

			return (void *)func;
		}
		template<typename ret> void *traverse(const ret& func(T&)) const {
			for (node *p = first(); p != tail; p = p->suc)
				func(p->data);

			return (void *)func;
		}
		template<typename ret> void *traverse(ret& func(const T&)) const {
			for (node *p = first(); p != tail; p = p->suc)
				func(p->data);

			return (void *)func;
		}
		template<typename ret> void *traverse(const ret& func(const T&)) const {
			for (node *p = first(); p != tail; p = p->suc)
				func(p->data);

			return (void *)func;
		}
		template<typename obj> obj& traverse(obj& functor) const {
			for (node *p = first(); p != tail; p = p->suc)
				functor(p->data);

			return functor;
		}
		Rank unique(Rank n = 0) {
			// i need to create a Rank, it could be like: "unique() {Rank r;}"
			// but it might seem more elegant if: "unique(Rank r = 0);"
			for (node *p = first(), *q; (q = p->suc) != tail && q != nullptr; )
				if (p->data == q->data) {
					remove(q);
					++n;
				} else
					p = p->suc;

			return n;
		}
		bool valid(const node *const& p)const {
			return p != nullptr && (p != head && p != tail);
		}
};
#endif
