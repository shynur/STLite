#include "STLite.hpp"
#include "test.hpp"
int main() {
    Vector<short> a{};
    List<short> l{};
    std::vector<short> b{};

    for (int i{0}, j{}; i < 3000'0000; ++i)
        j = std::rand(), a.push(j), l.push(j), b.push_back(j);

    std::cout << " heap_sort: " << bench([&] {
        a.sort("heap");
    }) << std::endl
              << "merge_sort: " << bench([&] {
                     l.sort("merge");
                 })
              << std::endl
              << " std::sort: " << bench([&] {
                     std::sort(b.begin(), b.end()), b;
                 })
              << std::endl;
}
