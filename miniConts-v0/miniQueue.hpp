#include "..\miniConts\miniList.hpp"
#include "..\miniConts\miniVector.hpp"
#ifndef MINI_QUEUE
#define MINI_QUEUE
template <typename T>
class miniQueue: protected miniList<T> {
	private:
		using Base = miniList<T>;
	protected:
		using typename Base::Rank;
		using Base::first;
        using Base::last;
		using Base::makelast;
		using Base::remove;
	public:
		miniQueue() = default;
		using Base::operator=;
		~miniQueue() = default;
	public:
		using Base::empty;
		using Base::size;
		miniQueue(const T& e) {
			this->push(e);
		}
		T push(const T& e) {
			return makelast(e)->data;
		}
		T pop(Rank *const p = nullptr) {
			if (T tmp; size() >= 1) {
				if (p != nullptr)
					*p = size() - 1;

				return remove(first());
			} else {
				if (p != nullptr)
					*p = 0;

				return tmp;
			}
		}
		T& front(Rank *const p = nullptr) const {
			if (p != nullptr)
				*p = size();

			return first()->data;
		}
		T& back(Rank *const p = nullptr) const {
			if (p != nullptr)
				*p = size();

			return last()->data;
		}
		void simulated_bank(const unsigned& n_wins,
							const unsigned& serve_time) const {
			struct Customer {
				unsigned win, time;
				unsigned& best_win(miniVector<miniQueue<Customer>>& wins) {
					for (unsigned i = win = 0, short_queue = wins[win].size();
							i < wins.size(); ++i)
						if (unsigned len = wins[i].size(); len < short_queue)
							win = i, short_queue = len;

					return win;
				}
			};
			miniVector<miniQueue<Customer>> wins(n_wins);

			for (unsigned now = 0; now < serve_time; ++now) {
				if (Customer c; std::rand() % (1 + n_wins) != 0) {
					c.time = 1 + std::rand() % 9;
					wins[c.best_win(wins)].push(c);
				}

				for (unsigned i = 0; i < n_wins; ++i)
					if (!wins[i].empty())
						if (--wins[i].front().time <= 0)
							wins[i].pop();

				for (unsigned i = 0; i < n_wins; ++i)
					if (!wins[i].empty())
						std::cout << wins[i].front().time << "|\n";
					else
						std::cout << " |\n";

				getchar();
				system("cls");
			}
		}
};
#endif
