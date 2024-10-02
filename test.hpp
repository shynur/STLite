#pragma once
#include <chrono>
template<typename F, typename... Args> auto bench(F&& f, Args... args) {
    std::chrono::system_clock::time_point start{std::chrono::system_clock::now()};
    f(args...);
    std::chrono::system_clock::time_point end{std::chrono::system_clock::now()};
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}
