#include <catch2/catch_test_macros.hpp>

#include "helpers.h"
#include "pipes.h"

TEST_CASE( "test_pipe_reverse()", "[pipe]" ) {
    auto v = std::vector<int>{3, 2, 1, 0} | mleivo::pipes::reverse();
    for (int i = 0; i < v.size(); ++i) {
        REQUIRE(i == v[i]);
    }
}

TEST_CASE( "test_pipe_for_each()", "[pipe]" ) {
    {
        auto square = [](int& i) { i *= i; };
        auto v = std::vector<int>{0, 1, 2, 3} | mleivo::pipes::for_each(square);
        for (int i = 0; i < v.size(); ++i) {
            REQUIRE(i * i == v[i]);
        }
    }
    {
        auto square = [](int& i) { i *= i; };
        auto w = std::vector<int>{0, 1, 2, 3};
        auto v = w | mleivo::pipes::for_each(square);
        for (int i = 0; i < v.size(); ++i) {
            REQUIRE(i * i == w[i]);
        }
    }
}

TEST_CASE( "test_pipe_for_each_verbose()", "[pipe]" ) {
    {
        auto f = [incr = 1](int& i) { i += incr; };
        auto x = mleivo::pipes::for_each(std::move(f));
        auto v = VerboseIntVector({0, 1, 2, 3}) | x | x | x;
        for (int i = 0; i < v.size(); ++i) {
            REQUIRE(i + 3 == v[i]);
        }
    }
    {
        auto f = [incr = VerboseIntVector{1}](int& i) { i += incr[0]; };
        auto x = mleivo::pipes::for_each(
            f); // NB: do _not_ std::move(f) here! if move is used, f gets moved -> incr
                // gets moved -> incr has size 0 in subsequent calls to x's operator() -> incr[0] is UB.
        auto v = VerboseIntVector({0, 1, 2, 3}) | x | x | x;
        for (int i = 0; i < v.size(); ++i) {
            REQUIRE(i + 3 == v[i]);
        }
    }
}

TEST_CASE( "test_pipe_sort()", "[pipe]" ) {
    auto v = std::vector<int>{3, 1, 2, 0} | mleivo::pipes::sort();
    for (int i = 0; i < v.size(); ++i) {
        REQUIRE(i == v[i]);
    }
}

TEST_CASE( "test_pipe_max_element()", "[pipe]" ) {
    std::cout << "\n";
    auto x = VerboseIntVector({0, 1, 2, 3});
    auto v = x | mleivo::pipes::max_element();
    REQUIRE(*v == 3);
}

TEST_CASE( "test_pipe_accumulate()", "[pipe]" ) {
    auto v = std::vector<int>{0, 1, 2, 3};
    auto ans = v | mleivo::pipes::accumulate(0, std::plus<int>{});
    REQUIRE(ans == 6);
}
