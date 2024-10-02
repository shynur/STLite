#include <iostream>
#ifndef MINI_VECTOR
#define MINI_VECTOR
template <typename T>
class miniVector {
	protected:
		using Rank = long;
		// Rank must be signed because of npos i.e. -1
		// int8_t may become a char
		Rank Rand() const {
			return std::rand();
		}
	protected:
		Rank _size;     // size
		Rank _capacity; // capacity
		T *_elem;       // data-area's address
		////////////////////////////////////////////////////////////////////////
		void bubbleSort(const Rank& lo, Rank hi) const {
			bool retry = true;

			while (retry && lo != --hi) {
				retry = false;

				for (Rank i = lo; i < hi; ++i)
					if (T tmp; _elem[i] > _elem[i + 1]) {
						retry = true;
						tmp = _elem[i];
						_elem[i] = _elem[i + 1];
						_elem[i + 1] = tmp;
					}
			}
		}
		void copyFrom(const T arr[], const Rank& lo, const Rank& hi) {
			_capacity = 2 * (hi - lo) + 1;
			_elem = new T[_capacity];

			for (_size = 0; lo + _size < hi; ++_size)
				_elem[_size] = arr[lo + _size];
		}
		void expand() {
			if (_size < _capacity)
				return;
			else
				// extend once size==capacity
				_capacity *= 2;

			T *old_elem = _elem;
			_elem = new T[_capacity];

			for (Rank i = 0; i < _size; ++i)
				_elem[i] = old_elem[i];

			delete[] old_elem;
		}
		void heapSort(Rank lo, Rank hi) const {
		}
		Rank max(Rank lo, Rank hi) const;
		Rank max() const {
			return max(0, _size);
		}
		void mergeSort(const Rank& lo, Rank hi) const {
			if (hi - lo <= 1)
				return;
			else {
				Rank mi = (lo + hi) / 2;
				// next 3 lines perform magic!
				miniVector<T> a(*this, lo, mi), b(*this, mi, hi);
				a.mergeSort(0, a.size());
				b.mergeSort(0, b.size());

				while (lo != hi--)
					if (a.empty() || b.empty())
						if (!a.empty())
							_elem[hi] = a.remove(a.size() - 1);
						else
							_elem[hi] = b.remove(b.size() - 1);
					else
						if (a[a.size() - 1] >= b[b.size() - 1])
							_elem[hi] = a.remove(a.size() - 1);
						else
							_elem[hi] = b.remove(b.size() - 1);
			}
		}
		Rank partition(Rank lo, Rank hi);
		void quickSort(Rank lo, Rank hi) const {
		}
		void shrink() {
			if (_size > _capacity / 4)
				return;
			else
				_capacity /= 2;

			T *old_elem = _elem;
			_elem = new T[_capacity];

			for (Rank i = 0; i < _size; ++i)
				_elem[i] = old_elem[i];

			delete[] old_elem;
		}
		void selectionSort(Rank lo, Rank hi) const {
		}
	public:
		miniVector(const Rank& siz = 0)
			: _capacity(siz + 1),
			  _elem(new T[siz + 1]) {
			for (_size = 0; _size < siz; ++_size)
				;
		}
		miniVector(const miniVector& another) {
			copyFrom(another._elem, 0, another._size);
		}
		virtual miniVector& operator=(const miniVector& another) {
			delete[] _elem;
			copyFrom(another._elem, 0, another._size);
			return *this;
		}
		virtual ~miniVector() {
			delete[] _elem;
		}
	public:
		miniVector(const Rank& siz,
				   const T& val): _capacity(siz + 1), _elem(new T[siz + 1]) {
			for (_size = 0; _size < siz; ++_size)
				_elem[_size] = val;
		}
		miniVector(const T arr[], const Rank& lo, const Rank& hi) {
			copyFrom(arr, lo, hi);
		}
		miniVector(const T arr[], const Rank& n) {
			copyFrom(arr, 0, n);
		}
		miniVector(const miniVector& another,
				   const Rank& lo, const Rank& hi) {
			copyFrom(another._elem, lo, hi);
		}
		Rank disordered() const {
			Rank cnt = 0;

			for (Rank i = 0; i <= _size - 2; ++i)
				if (_elem[i] > _elem[i + 1])
					++cnt;

			return cnt;
		}
		bool empty() const {
			if (_size >= 1)
				return false;
			else
				return true;
		}
		Rank find(const T& e) const {
			return find(e, 0, _size);
		}
		Rank find(const T& e, const Rank& lo, Rank hi) const {
			while (--hi >= lo)
				if (_elem[hi] == e)
					return hi;

			return -1;
		}
		bool operator>(const miniVector& another) const {
			Rank i = -1;
			Rank short_tail = (_size > another._size ? another._size : _size);

			while (++i != short_tail)
				if (_elem[i] != another[i])
					break;

			if (i < short_tail)
				return _elem[i] > another[i];
			else
				return _size > another._size;
		}
		bool operator==(const miniVector& another) const {
			if (_size != another._size)
				return false;
			else {
				for (Rank i = 0; i < _size; ++i)
					if (_elem[i] != another[i])
						return false;

				return true;
			}
		}
		bool operator>=(const miniVector& another) const {
			return *this > another || *this == another;
		}
		bool operator<=(const miniVector& another) const {
			return another > *this;
		}
		bool operator<(const miniVector& another) const {
			return another >= *this;
		}
		Rank size() const {
			return _size;
		}
		Rank search(const T& e) const {
			return search(e, 0, _size);
		}
		Rank search(const T& e, Rank lo, Rank hi) const {
			while (lo < hi)

				// [0, lo)'s every elem <= e < [hi, size)'s
				if (Rank mi = (lo + hi) / 2; _elem[mi] <= e)
					lo = mi + 1;
				else
					hi = mi;

			return --lo;
		}
		/*--- setter ---*/
		Rank dedup() {
			Rank diff = 0;

			for (Rank i = 1; i < _size;)
				if (find(_elem[i], 0, i) == -1)
					++i;
				else {
					remove(i);
					++diff;
				}

			return diff;
		}
		Rank insert(const Rank i, const T& e) {
			expand();

			for (Rank j = _size; j > i; --j)
				_elem[j] = _elem[j - 1];

			++_size;
			_elem[i] = e;
			return i;
		}
		Rank insert(const T& e) {
			return insert(_size, e);
		}
		T& operator[](const Rank& i) const {
			return _elem[i % _size];
		}
		T remove(const Rank& i = 0) {
			T e = _elem[i];
			remove(i, i + 1);
			return e;
		}
		Rank remove(Rank lo, Rank hi) {
			while (hi < _size)
				_elem[lo++] = _elem[hi++];

			_size = lo;
			shrink();
			return hi - lo;
		}
		void reverse() const {
			for (Rank lo = 0, hi = _size - 1; lo < hi; ++lo, --hi) {
				T lower = _elem[lo];
				_elem[lo] = _elem[hi];
				_elem[hi] = lower;
			}
		}
		void sort(const Rank& lo, const Rank& hi,
				  const Rank& choice = 0) const {
			switch (choice == 0 ? 1 + Rand() % 5 : choice) {
				case 1:
					bubbleSort(lo, hi);
					break;

				case 2:
					selectionSort(lo, hi);
					break;

				case 3:
					mergeSort(lo, hi);
					break;

				case 4:
					heapSort(lo, hi);
					break;

				case 5:
					quickSort(lo, hi);
					break;
			}
		}
		void sort(const Rank& choice = 0) const {
			sort(0, _size, choice);
		}
		template<typename ret> void *traverse(ret func(T)) const {
			for (Rank i = 0; i < _size; ++i)
				func(_elem[i]);

			return (void *)func;
		}
		template<typename ret> void *traverse(ret& func(T)) const {
			for (Rank i = 0; i < _size; ++i)
				func(_elem[i]);

			return (void *)func;
		}
		template<typename ret> void *traverse(const ret& func(T)) const {
			for (Rank i = 0; i < _size; ++i)
				func(_elem[i]);

			return (void *)func;
		}
		template<typename ret> void *traverse(ret func(T&)) const {
			for (Rank i = 0; i < _size; ++i)
				func(_elem[i]);

			return (void *)func;
		}
		template<typename ret> void *traverse(ret func(const T&)) const {
			for (Rank i = 0; i < _size; ++i)
				func(_elem[i]);

			return (void *)func;
		}
		template<typename ret> void *traverse(ret& func(T&)) const {
			for (Rank i = 0; i < _size; ++i)
				func(_elem[i]);

			return (void *)func;
		}
		template<typename ret> void *traverse(const ret& func(T&)) const {
			for (Rank i = 0; i < _size; ++i)
				func(_elem[i]);

			return (void *)func;
		}
		template<typename ret> void *traverse(ret& func(const T&)) const {
			for (Rank i = 0; i < _size; ++i)
				func(_elem[i]);

			return (void *)func;
		}
		template<typename ret> void *traverse(const ret& func(const T&)) const {
			for (Rank i = 0; i < _size; ++i)
				func(_elem[i]);

			return (void *)func;
		}
		template <typename obj> obj& traverse(obj& functor) const {
			for (Rank i = 0; i < _size; ++i)
				functor(_elem[i]);

			return functor;
		}
		Rank unique() {
			// it will change size
			Rank i = 0, j = 0;

			while (++j < _size)
				if (_elem[i] == _elem[j])
					continue;
				else
					_elem[++i] = _elem[j];

			_size = ++i;
			shrink();
			return j - i;
		}
		void unsort(const Rank& lo, Rank hi) const {
			while (lo != hi--) {
				Rank i = Rand() % (hi - lo);
				T tmp = _elem[lo + i];
				_elem[lo + i] = _elem[hi];
				_elem[hi] = tmp;
			}
		}
		void unsort() {
			unsort(0, _size);
		}
};
#endif
