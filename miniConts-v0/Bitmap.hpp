#include <iostream>
#include <fstream>
#include "..\miniConts\miniVector.hpp"
#ifndef MINI_BITMAP
#define MINI_BITMAP
template<> class miniVector<bool>;
template<> class miniVector<bool> {
	protected:
		using Rank = unsigned long;
		using Bitmap = miniVector<bool>;
		Rank n;       // like mole, n = N/N[A]. here, n = (N+7)/8
		char *_byte;
	protected:
		bool if_first_time = true;
		Rank last_rank;
		bool last_visited;
	protected:
		Rank expand(const Rank& i, const bool& flag = false) {
			if (Rank old_n = n; i < old_n * 8)
				return 0;
			else {
				n = (i / 8 + 1) * 2;
				char *old_byte = _byte;
				_byte = new char[n];

				for (Rank j = 0; j < old_n; ++j)
					_byte[j] = old_byte[j];

				delete[] old_byte;
				membat(old_n * 8, n * 8, flag);
				return (n - old_n) * 8;
			}
		}
	public:
		miniVector(const Rank& N = 8, const bool& flag = false)
			: n((N + 7) / 8), _byte(new char[n]) {
			membat(0, 8 * n, flag);
		}
		miniVector(const Bitmap& another)
			: n(another.n), _byte(new char[n]) {
			for (Rank p = 0; p < n; ++p)
				_byte[p] = (another._byte)[p];
		}
		virtual Bitmap& operator=(const Bitmap& another) {
			delete[] _byte;
			n = another.n;
			_byte = new char[n];

			for (Rank p = 0; p < n; ++p)
				_byte[p] = (another._byte)[p];

			return *this;
		}
		virtual ~miniVector() {
			delete[] _byte;
		}
	public:
		miniVector(const char *const& fname, Rank n = 0)
			: n(n), _byte(new char[n]) {
			std::ifstream ifs(fname, std::ios_base::in | std::ios_base::binary);

			if (ifs.is_open()) {
				if (n == 0) {
					ifs.seekg(0, std::ios::end);
					(n = ifs.tellg()) >= 1 ? expand(n * 8 - 1, false) : 0;
					ifs.seekg(0, std::ios::beg);
				}

				ifs.read(_byte, n);
				ifs.close();
			} else {
				std::cout << "\aUnable to open input file: " << fname << '\n'
						  << "Press any key to exit...";
				getchar();
			}
		}
		Rank membat(const Rank& l, const Rank& h, const bool& flag = false) {
			if (h >= 1)
				expand(h - 1, false);

			if (flag)
				for (Rank i = l; i < h; ++i)
					set(i);
			else
				for (Rank i = l; i < h; ++i)
					clear(i);

			return h - l;
		}

		bool set(const Rank& i, bool old_flag = false) {
			expand(i, false);
			old_flag = test(i);
			_byte[i / 8] |= (1 << (i % 8));
			return old_flag;
		}
		bool clear(const Rank& i, bool old_flag = false) {
			expand(i, false);
			old_flag = test(i);
			_byte[i / 8] &= ~(1 << (i % 8));
			return old_flag;
		}
		bool test(const Rank& i) {
			expand(i, false);
			return _byte[i / 8] & (1 << (i % 8));
		}
		bool& operator[](const Rank& i) {
			if (if_first_time)
				if_first_time = false;
			else
				if (last_visited)
					set(last_rank);
				else
					clear(last_rank);

			return last_visited = test(last_rank = i);
		}

		Rank dump(const char *const& fname,
				  const bool& only_ascii = false, Rank m = 0) const {
			std::ofstream ofs(fname,
							  std::ios_base::out | std::ios_base::binary);

			if (m = n; only_ascii)
				while (m >= 1 && _byte[m - 1] == '\0')
					--m;

			if (ofs.is_open()) {
				ofs.write(_byte, m);
				ofs.close();
			} else {
				std::cout << "\aUnable to open output file: " << fname << '\n'
						  << "Press any key to exit...";
				getchar();
			}

			return m;
		}
		Rank len(Rank *const& plen = nullptr) const {
			if (plen == nullptr)
				return n;
			else
				return *plen = n;
		}
		char *literal(Rank N = 0) {
			if (N == 0)
				N = n * 8;
			else
				expand(N, false);

			char *s = new char[N + 1];
			s[N] = '\0';

			for (Rank i = 0; i < N; ++i)
				s[i] = test(i) ? '1' : '0';

			return s;
		}
};
using Bitmap = miniVector<bool>;
#endif
