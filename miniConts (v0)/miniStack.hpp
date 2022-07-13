#include "..\miniConts\miniVector.hpp"
#ifndef MINI_STACK
#define MINI_STACK
template <typename T>
class miniStack: protected miniVector<T> {
	private:
		using Base = miniVector<T>;
	protected:
		using typename Base::Rank;
	public:
		miniStack() = default;
		using Base::operator=;
		~miniStack() = default;
	public:
		using Base::find;   // shit!
		using Base::empty;
		using Base::size;
		miniStack(const T& e) {
			this->push(e);
		}
		Rank push(const T& e) {
			return Base::insert(e);
		}
		T pop() {
			if (static T tmp; size() >= 1)
				return tmp = Base::remove(size() - 1);
			else
				return tmp;
		}
		T& top() const {
			if (static T tmp; size() >= 1)
				return (*this)[size() - 1];
			else
				return tmp;
		}
	public:
		unsigned convert(unsigned long long n, const unsigned& base) {
			static const char digit[] = {
				'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
				'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
				'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
				'U', 'V', 'W', 'X', 'Y', 'Z',
			};
			unsigned cnt = 0;

			do {
				push(digit[n % base]);
				n /= base;
				++cnt;
			} while (n); // remainder != 0

			return cnt;
		}

		miniStack<unsigned> EightQueens(const unsigned& n = 4) const {
			/// N Queens
			struct Queen {
				unsigned x, y;
				Queen(unsigned x = 1, unsigned y = 1): x(x), y(y) {}
				bool operator==(const Queen& another) const {
					return (x == another.x || y == another.y)
						   || x + y == another.x + another.y
						   || x + another.y == y + another.x;
				}
			};
			miniStack<Queen> qs; // queens
			int ww;

			for (Queen q; qs.size() < n; q.x = 1)
				for ( q.x = 1; q.x <= n; ++q.x)
					if (qs.find(q) == -1) {
						qs.push(q);
						++q.y;
						break;
					} else
						if (q.x != n)
							continue;
						else
							do {
								q = qs.pop();
							} while (q.x + 1 > n);

			miniStack<unsigned> ans;

			while (!qs.empty())
				ans.push(qs.pop().x);

			return ans;
		}
		void TheseusSaveAriadne_row_col_map_Ti_Tj_Ai_Aj(
			const unsigned& m,
			const unsigned& n,
			const bool *const& map_of_laby,
			unsigned i, unsigned j,
			const unsigned& Ai, const unsigned& Aj,
			const bool& mov_manually = true
		) const {
			// need to include <iostream>
			enum Dir {
				Up, Right, Down, Left, End,
			};
			enum State {
				Plain,
				Wall,
				Route,
				Abandon,
			};
			struct cell {
				State now = Plain;
				Dir src = Up, dest = Up;
				void show() const {
					// need Unicode; "#define _O_WTEXT 0x10000" in <fcnth.h>
					_setmode(_fileno(stdout), 0x10000);

					if (now != Route || dest == End)
						if (dest == End && now != Wall)
							std::wcout << L'\x25CC';
						else {
							if (now == Wall)
								std::wcout << L'\x2588';

							if (now == Plain)
								std::wcout << L'\x0020';

							if (now == Abandon)
								std::wcout << L'\x25E6';
						} else
						if (src != End)
							if (src != dest) {
								if ((src == Up && dest == Right)
										|| (src == Right && dest == Up))
									std::wcout << L'\x2570';

								if ((src == Right && dest == Down)
										|| (src == Down && dest == Right))
									std::wcout << L'\x256D';

								if ((src == Down && dest == Left)
										|| (src == Left && dest == Down))
									std::wcout << L'\x256E';

								if ((src == Left && dest == Up)
										|| (src == Up && dest == Left))
									std::wcout << L'\x256F';

								if ((src == Up && dest == Down)
										|| (src == Down && dest == Up))
									std::wcout << L'\x2502';

								if ((src == Left && dest == Right)
										|| (src == Right && dest == Left))
									std::wcout << L'\x2500';
							} else {
								if (src == Up)
									std::wcout << L'\x2575';

								if (src == Right)
									std::wcout << L'\x2576';

								if (src == Down)
									std::wcout << L'\x2577';

								if (src == Left)
									std::wcout << L'\x2574';
							} else {
							if (dest == Up)
								std::wcout << L'\x2575';

							if (dest == Right)
								std::wcout << L'\x2576';

							if (dest == Down)
								std::wcout << L'\x2577';

							if (dest == Left)
								std::wcout << L'\x2574';
						}

					// need ASCII; "#define _O_TEXT 0x4000" in <fcnth.h>
					_setmode(_fileno(stdout), 0x4000);
				}
			};
			cell *laby[m];
			struct Put_laby {
				void operator()(cell *laby_puted[],
								const unsigned& laby_row,
								const unsigned& laby_col,
								const bool& put_laby_manually = true) const {
					std::cout << "\nPress any key to continue...";

					if (put_laby_manually)
						getchar();

					system("cls");

					for (unsigned i = 0; i < laby_row; ++i) {
						for (unsigned j = 0; j < laby_col; ++j)
							laby_puted[i][j].show();

						std::cout << '\n';
					}
				}
			} put_laby;

			for (cell *&laby_row : laby)
				laby_row = new cell[n];

			{
				// init_laby
				for (unsigned init_i = 0; init_i < m; ++init_i)
					for (unsigned init_j = 0; init_j < n; ++init_j)
						laby[init_i][init_j].now =
							map_of_laby[init_i * n + init_j] ? Plain : Wall;

				laby[Ai][Aj].dest = End;
				laby[i][j].src = End;
			}
			{
				// Theseus go!
				for (put_laby(laby, m, n, mov_manually), laby[i][j].now = Route;
						laby[i][j].dest != End;
						laby[i][j].now = Route) {
					put_laby(laby, m, n, mov_manually);

					switch (laby[i][j].dest) {
						case Up:
							if (laby[i - 1][j].now == Plain) {
								i = i - 1;
								laby[i][j].src = Down;
							} else
								laby[i][j].dest = Right;

							break;

						case Right:
							if (laby[i][j + 1].now == Plain) {
								j = j + 1;
								laby[i][j].src = Left;
							} else
								laby[i][j].dest = Down;

							break;

						case Down:
							if (laby[i + 1][j].now == Plain) {
								i = i + 1;
								laby[i][j].src = Up;
							} else
								laby[i][j].dest = Left;

							break;

						case Left:
							if (laby[i][j - 1].now == Plain) {
								j = j - 1;
								laby[i][j].src = Right;
							} else {
								laby[i][j].now = Abandon;

								switch (laby[i][j].src) {
									case Up:
										i = i - 1;
										break;

									case Right:
										j = j + 1;
										break;

									case Down:
										i = i + 1;
										break;

									case Left:
										j = j - 1;
										break;
								}
							}

							break;
					}
				}

				put_laby(laby, m, n);
			}
		}
};
#endif
