#ifndef TEST_CPP
#define TEST_CPP
#include "..\test.hpp"
void test_nocopyable();
void run_against_std();
template<typename Map>Map& show_info(String, Map&);
inline int rand_1000_0000();
template<typename Map>inline void test_find(const Map&);
int main() {
    test_nocopyable();
    cout << endl;
    run_against_std();
}
void run_against_std() {
    String start{"--------------- my Hash's performance against std::unordered_map ---------------\n"};
    cout << start << endl;
    Hash<int, string> a{};
    unordered_map<int, string> b{};
    static const constexpr auto rand_pair_gener{[]()->auto{
            static const char *_nums[] {"zero-", "one-", "two-", "three-", "four-", "five-", "six-", "seven-", "eight-", "nine-"};
            static const Vector<string> nums{_nums};
            int n{rand_1000_0000()}, m{n};
            string s{};

            do {
                s = nums[m % 10] + s, m /= 10;
            } while (m);

            s.pop_back();
            return pair{n, s};
        }};
    vector<pair<int, string>>vec{};

    while (vec.size() != 1000'0000)
        vec.push_back(rand_pair_gener());

    static size_t a_insertion_time{0}, b_insertion_time{0};

    for (int i{0}; i < 3000'0000; ++i) {
        const auto& [n, s] {vec[rand_1000_0000()]};
        a_insertion_time += bench([&] {a.insert(n, s);});
        b_insertion_time += bench([&] {b.insert({n, s});});
    }

    cout << "3000,0000 insertion (maybe repeatedly)\n"
         << "      my Hash insertion_time: " << a_insertion_time << " ms\n"
         << "unordered_map insertion_time: " << setw(5) << b_insertion_time << " ms\n";
    show_info("      my Hash", a), show_info("unordered_map", b), cout << endl;
    test_find(a);
    static size_t a_find_time{0}, b_find_time{0};
    // *INDENT-OFF*
    for (int i{0}; i < 2000'0000; ++i) {
        const auto& [n, s] {vec[rand_1000_0000()]};
        a_find_time += bench([&] {try {a[n];} catch(...) {}});
        b_find_time += bench([&] {*b.find(n);});
    }
    // *INDENT-ON*
    cout << "2000,0000 finding (maybe repeatedly)\n"
         << "      my Hash find_time: " << a_find_time << " ms\n"
         << "unordered_map find_time: " << setw(4) << b_find_time << " ms\n\n";
    static size_t a_erase_time{0}, b_erase_time{0};
    // *INDENT-OFF*
    for (int i{0}; i < 4500'0000; ++i) {
        const auto& [n, s] {vec[rand_1000_0000()]};
        a_erase_time += bench([&] {try {a.pick(n);} catch(...) {}});
        b_erase_time += bench([&] {b.erase(n);});
    }
    // *INDENT-ON*
    cout << "4500,0000 erasion (maybe repeatedly)\n"
         << "      my Hash erase_time: " << a_erase_time << " ms\n"
         << "unordered_map erase_time: " << setw(5) << b_erase_time << " ms\n";
    show_info("      my Hash", a), show_info("unordered_map", b);
    cout << "\nshow all key-value pairs in my Hash", getchar();

    for (static int i{0}; const auto& [key, val] : a)
        cout << "No." << setw(5) << ++i << "    " << setw(7) << key << "    " << val << '\n';

    cout << "\nprepare for next test. insert till size = 500,0000\n";

    while (a.size() != 500'0000) {
        auto [n, s] {vec[rand_1000_0000()]};
        a.insert(n, s);
    }

    test_find(show_info("      my Hash", a));
}
void test_nocopyable() {
    uString start{"----------------- start to test Hash<Key_t, nocopyable_Val_t> -----------------\n"};
    cout << start << endl;
    Hash<String, thread> a{}, b{};
    static const constexpr char *_words[] {"123", "456", "789"};
    Vector<uString> words{_words}; // uString can be converted to String~

    for (cout << "insert for a"; const auto& word : words)
        a.insert(word, thread{[]{}});
    getchar(), b = move(show_info("a", a));
    cout << "\nafter move a to b", getchar(), show_info("a", a), show_info("b", b);
    cout << "\nb has key-val with key[\"123\"]? ", getchar(), cout << boolalpha << (b & "123") << '\n';
    b["789"].join(), b.insert("789", thread{}).insert("012", thread{[]{}});
    Hash c{move(show_info("b", b))};
    cout << "\nafter move b to c", getchar(), show_info("a", a), show_info("b", b), show_info("c", c);

    for (auto& [s, t] : c)
        if (s != String{"789"})
            t.join();
}
template<typename Map>Map& show_info(String s, Map& some_map) {
    cout << s + ": size = " << some_map.size() << ", load_factor(%) = " << 100.0 * some_map.load_factor() << endl;
    return some_map;
}
inline int rand_1000_0000() {
    auto a{rand()}, b{rand()};

    if (rand() ^ 1)
        swap(a, b);

    return a % 1000 * 10000 + b % 10000;
}
template<typename Map>inline void test_find(const Map& some_map) {
    cout << "--------------------------------------------------------------------------------\n";
    cin.clear(), cin.sync();

    for (unsigned i{}; cin >> i; cout << endl)
        if (some_map & i)
            cout << some_map[i] << '\n';
        else
            cout << "entry with given key does NOT exist\n";

    cin.clear(), cin.sync();
    cout << "--------------------------------------------------------------------------------\n";
}
#endif
