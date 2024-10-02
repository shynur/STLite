#include "test.hpp"
void test_nocopyable();
void run_against_std();
template<typename Map> Map& show_info(String, Map&);
inline int rand_1000_0000();
template<typename Map> inline void test_find(const Map&);
int main() {
    test_nocopyable();
    std::cout << std::endl;
    run_against_std();
}
void run_against_std() {
    std::String start{"--------------- my Hash's performance against std::unordered_map ---------------\n"};
    std::cout << start << std::endl;
    Hash<int, std::string> a{};
    unordered_map<int, std::string> b{};
    static constexpr const auto rand_pair_gener{[]() -> auto {
        static const char* _nums[]{"zero-", "one-", "two-", "three-", "four-", "five-", "six-", "seven-", "eight-", "nine-"};
        static const Vector<std::string> nums{_nums};
        int n{rand_1000_0000()}, m{n};
        std::string s{};

        do {
            s = nums[m % 10] + s, m /= 10;
        } while (m);

        s.pop_back();
        return std::pair{n, s};
    }};
    vector<pair<int, std::string>> vec{};

    while (vec.size() != 1000'0000)
        vec.push_back(rand_pair_gener());

    static size_t a_insertion_time{0}, b_insertion_time{0};

    for (int i{0}; i < 3000'0000; ++i) {
        const auto& [n, s]{vec[rand_1000_0000()]};
        a_insertion_time += bench([&] {
            a.insert(n, s);
        });
        b_insertion_time += bench([&] {
            b.insert({n, s});
        });
    }

    std::cout << "3000,0000 insertion (maybe repeatedly)\n"
              << "      my Hash insertion_time: " << a_insertion_time << " ms\n"
              << "unordered_map insertion_time: " << std::setw(5) << b_insertion_time << " ms\n";
    show_info("      my Hash", a), show_info("unordered_map", b), std::cout << std::endl;
    test_find(a);
    static size_t a_find_time{0}, b_find_time{0};
    // *INDENT-OFF*
    for (int i{0}; i < 2000'0000; ++i) {
        const auto& [n, s]{vec[rand_1000_0000()]};
        a_find_time += bench([&] {
            try {
                a[n];
            } catch (...) {}
        });
        b_find_time += bench([&] {
            *b.find(n);
        });
    }
    // *INDENT-ON*
    std::cout << "2000,0000 finding (maybe repeatedly)\n"
              << "      my Hash find_time: " << a_find_time << " ms\n"
              << "unordered_map find_time: " << std::setw(4) << b_find_time << " ms\n\n";
    static size_t a_erase_time{0}, b_erase_time{0};
    // *INDENT-OFF*
    for (int i{0}; i < 4500'0000; ++i) {
        const auto& [n, s]{vec[rand_1000_0000()]};
        a_erase_time += std::bench([&] {
            try {
                a.pick(n);
            } catch (...) {}
        });
        b_erase_time += std::bench([&] {
            b.erase(n);
        });
    }
    // *INDENT-ON*
    std::cout << "4500,0000 erasion (maybe repeatedly)\n"
              << "      my Hash erase_time: " << a_erase_time << " ms\n"
              << "unordered_map erase_time: " << std::setw(5) << b_erase_time << " ms\n";
    show_info("      my Hash", a), show_info("unordered_map", b);
    std::cout << "\nshow all key-value pairs in my Hash", std::getchar();

    for (static int i{0}; const auto& [key, val] : a)
        std::cout << "No." << std::setw(5) << ++i << "    " << std::setw(7) << key << "    " << val << '\n';

    std::cout << "\nprepare for next test. insert till size = 500,0000\n";

    while (a.size() != 500'0000) {
        auto [n, s]{vec[rand_1000_0000()]};
        a.insert(n, s);
    }

    test_find(show_info("      my Hash", a));
}
void test_nocopyable() {
    uString start{"----------------- start to test Hash<Key_t, nocopyable_Val_t> -----------------\n"};
    std::cout << start << std::endl;
    Hash<String, thread> a{}, b{};
    static constexpr const char* _words[]{"123", "456", "789"};
    Vector<uString> words{_words}; // uString can be converted to String~

    for (std::cout << "insert for a"; const auto& word : words)
        a.insert(word, std::thread{[] {}});
    std::getchar(), b = move(show_info("a", a));
    std::cout << "\nafter move a to b", std::getchar(), show_info("a", a), show_info("b", b);
    std::cout << "\nb has key-val with key[\"123\"]? ", std::getchar(), std::cout << std::boolalpha << (b & "123") << '\n';
    b["789"].join(), b.insert("789", std::thread{}).insert("012", std::thread{[] {}});
    Hash c{move(show_info("b", b))};
    std::cout << "\nafter move b to c", std::getchar(), show_info("a", a), show_info("b", b), show_info("c", c);

    for (auto& [s, t] : c)
        if (s != String{"789"})
            t.join();
}
template<typename Map> Map& show_info(String s, Map& some_map) {
    std::cout << s + ": size = " << some_map.size() << ", load_factor(%) = " << 100.0 * some_map.load_factor() << std::endl;
    return some_map;
}
inline int rand_1000_0000() {
    auto a{std::rand()}, b{std::rand()};

    if (std::rand() ^ 1)
        swap(a, b);

    return a % 1000 * 1'0000 + b % 1'0000;
}
template<typename Map> inline void test_find(const Map& some_map) {
    std::cout << "--------------------------------------------------------------------------------\n";
    std::cin.clear(), std::cin.sync();

    for (unsigned i{}; std::cin >> i; std::cout << std::endl)
        if (some_map & i)
            std::cout << some_map[i] << '\n';
        else
            std::cout << "entry with given key does NOT exist\n";

    std::cin.clear(), std::cin.sync();
    std::cout << "--------------------------------------------------------------------------------\n";
}
