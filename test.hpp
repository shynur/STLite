#ifndef TEST_HPP
#define TEST_HPP
#include "Conts.hpp"
#include <bits/stdc++.h>
namespace debug {
    // *INDENT-OFF*
    template<typename T, typename...rest_T>inline void dbg(const T& info, const rest_T&...rest_infos) {if constexpr (sizeof...(rest_infos) == 0) ::std::cerr << info << " "; else dbg(info), dbg(rest_infos...);}
    template<typename F, typename...Args>inline auto bench(const F& cref_f, Args...args) {
        std::chrono::system_clock::time_point start{std::chrono::system_clock::now()};

        const_cast<F&>(cref_f)(args...);

        std::chrono::system_clock::time_point end{std::chrono::system_clock::now()};
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }
} using namespace debug;
using namespace std;
#endif
