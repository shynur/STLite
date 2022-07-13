#ifndef TEST_CPP
#define TEST_CPP
#include "test.hpp"
int main() {
    Vector<short> a{};
    List<short> l{};
    vector<short> b{};

    for (int i{0}, j{}; i < 3000'0000; ++i)
        j = rand(), a.push(j), l.push(j), b.push_back(j);
    cout << " heap_sort: " << bench([&] {a.sort("heap");}) << endl
         << "merge_sort: " << bench([&] {l.sort("merge");}) << endl
         << " std::sort: " << bench([&] {sort(b.begin(), b.end()), b;}) << endl;
}
#endif