#include <catch2/catch.hpp>
#include <deque>
#include <set>

#include "containerutils.h"
#include "helpers.h"

TEST_CASE( "test_remove_all()" , "[container utils]" ) {
    {
        std::vector<int> v{1, 2, 1, 2, 1};
        mleivo::cu::remove_all(v, 1);
        REQUIRE(true == cmp(std::vector<int>{2, 2}, v));
        mleivo::cu::remove_all(v, 2);
        REQUIRE(0 == v.size());
    }
    {
        std::vector<int> v{0, 1, 2, 3, 4, 5};
        mleivo::cu::remove_all(v, [](int i) { return i % 2 == 0; });
        REQUIRE(true == cmp(std::vector<int>{1, 3, 5}, v));
    }
}

TEST_CASE( "test_move_to_index()" , "[container utils]" ) {
    {
        std::vector<int> v{3, 1, 2, 0};
        mleivo::cu::move_to_index(v, 3, 0);
        REQUIRE(true == cmp(std::vector<int>{0, 3, 1, 2}, v));

        mleivo::cu::move_to_index(v, 1, 3);
        REQUIRE(true == cmp(std::vector<int>{0, 1, 2, 3}, v));

        mleivo::cu::move_to_index(v, 1, 1);
        REQUIRE(true == cmp(std::vector<int>{0, 1, 2, 3}, v));
    }
}

TEST_CASE( "test_pop_front()" , "[container utils]" ) {
    {
        std::vector<int> v{0, 1, 2};
        mleivo::cu::pop_front(v);
        REQUIRE(1 == v.front());
        REQUIRE(2 == v.size());

        mleivo::cu::pop_front(v);
        REQUIRE(1 == v.size());
        REQUIRE(2 == v.front());

        mleivo::cu::pop_front(v);
        REQUIRE(0 == v.size());
    }
}

TEST_CASE( "test_static_cast_all()" , "[container utils]" ) {
    {
        auto v = std::vector<double>{0, 1, 2};
        auto ints = mleivo::cu::static_cast_all<int>(v);
        static_assert(std::is_same_v<decltype(ints), std::vector<int>>);
        REQUIRE(3 == ints.size());
        for (int i = 0; i < ints.size(); ++i) {
            REQUIRE(i == ints[i]);;
        }
    }
    {
        auto v = std::deque<double>{0, 1, 2};
        auto ints = mleivo::cu::static_cast_all<int>(v);
        static_assert(std::is_same_v<decltype(ints), std::deque<int>>);
        REQUIRE(3 == ints.size());
        mleivo::cu::contains(ints, 0);
        mleivo::cu::contains(ints, 1);
        mleivo::cu::contains(ints, 2);
    }
    {
        double v[3] = {0, 1, 2};
        auto ints = mleivo::cu::static_cast_all<int>(v);
        static_assert(std::is_same_v<decltype(ints), std::array<int, 3>>);
        REQUIRE(3 == ints.size());
        for (int i = 0; i < ints.size(); ++i) {
            REQUIRE(i == ints[i]);
        }
    }
    {
        std::array<double, 3> v = {0, 1, 2};
        auto ints = mleivo::cu::static_cast_all<int>(v);
        static_assert(std::is_same_v<decltype(ints), std::array<int, 3>>);
        REQUIRE(3 == ints.size());
        for (int i = 0; i < ints.size(); ++i) {
            REQUIRE(i == ints[i]);
        }
    }

    {
        auto as_base = std::vector<Base*>{};
        as_base.emplace_back(new Derived);
        as_base.emplace_back(new Derived);
        as_base.emplace_back(new Derived);

        for (const auto* b : as_base) {
            REQUIRE((*b)() == 0xB);
        }

        auto as_der = mleivo::cu::static_cast_all<Derived*>(as_base);
        for (const auto* d : as_der) {
            REQUIRE((*d)() == 0xD);
        }

        for (auto* d : as_base) {
            delete d;
        }
    }
}

TEST_CASE( "test_contains()", "container utils" ) {
    struct wrapper : public std::vector<int> {
        bool contains(int i) const {
            return std::find(std::cbegin(*this), std::cend(*this), i) != std::cend(*this);
        }
    };

    {
        auto s = std::set<int>{0, 1, 2, 3};
        REQUIRE(true == mleivo::cu::contains(s, 0));
        REQUIRE(false == mleivo::cu::contains(s, -1));
    }

    {
        auto w = wrapper{};
        w.emplace_back(0);
        w.emplace_back(1);
        w.emplace_back(2);
        REQUIRE(true == mleivo::cu::contains(w, 0));
        REQUIRE(false == mleivo::cu::contains(w, -1));
    }

    {
        auto v = std::vector<int>{0, 1, 2, 3, 4};
        REQUIRE(true == mleivo::cu::contains(v, 0));
        REQUIRE(false == mleivo::cu::contains(v, -1));
    }

    {
        std::vector<std::unique_ptr<int>> v;
        v.emplace_back(std::make_unique<int>(0));
        v.emplace_back(std::make_unique<int>(1));
        v.emplace_back(std::make_unique<int>(2));
        v.emplace_back(std::make_unique<int>(3));

        REQUIRE(true == mleivo::cu::contains(v, v.front()));
        REQUIRE(false == mleivo::cu::contains(v, nullptr));
        REQUIRE(false == mleivo::cu::contains(v, std::make_unique<int>(0)));
    }
}

TEST_CASE( "test_merge()", "container utils" ) {
    {
        std::vector<int> v1{0, 1};
        std::set<int> v2{2, 3};
        std::deque<int> v3{4, 5};

        const auto merged = mleivo::cu::merge(v1, v2, v3);
        REQUIRE(merged.size() == 6);
        for (int i = 0; i < merged.size(); i++) {
            REQUIRE(i == merged[i]);
        }
    }

    {
        std::vector<move_only_type> v1;
        std::vector<move_only_type> v2;
        std::deque<move_only_type> v3;

        v1.emplace_back(0);
        v1.emplace_back(1);
        v2.emplace_back(2);
        v2.emplace_back(3);
        v3.emplace_back(4);
        v3.emplace_back(5);

        const auto merged = mleivo::cu::merge(std::move(v1), std::move(v2), std::move(v3));
        REQUIRE(merged.size() == 6);
        for (int i = 0; i < merged.size(); i++) {
            REQUIRE(i == *merged[i].m_val);
        }
    }

    {
        std::vector<copy_only_type> v1;
        std::vector<copy_only_type> v2;
        std::deque<copy_only_type> v3;

        v1.emplace_back(0);
        v1.emplace_back(1);
        v2.emplace_back(2);
        v2.emplace_back(3);
        v3.emplace_back(4);
        v3.emplace_back(5);

        const auto merged = mleivo::cu::merge(std::move(v1), std::move(v2), std::move(v3));
        REQUIRE(merged.size() == 6);
        for (int i = 0; i < merged.size(); i++) {
            REQUIRE(i == merged[i].m_val);
        }
    }
}

TEST_CASE( "test_filter()", "container utils" ) {
    {
        auto ms = std::multiset<int>{0, 1, 2, 3, 4, 5};
        auto even = mleivo::cu::filter(ms, [](const auto& i) { return i % 2 == 0; });
        REQUIRE(even.size() == 3);
        for (int i = 0; i < even.size(); i++) {
            REQUIRE(i * 2 == even[i]);
        }
    }

    {
        auto v = std::vector<move_only_type>{};
        v.emplace_back(5);
        v.emplace_back(4);
        v.emplace_back(3);
        v.emplace_back(2);
        v.emplace_back(1);
        v.emplace_back(0);
        auto even = mleivo::cu::filter(std::move(v), [](const auto& i) { return *i.m_val % 2 == 0; });
        REQUIRE(even.size() == 3);
        std::sort(even.begin(), even.end());
        for (int i = 0; i < even.size(); i++) {
            REQUIRE(i * 2 == *even[i].m_val);
        }
    }

    {
        auto v = std::vector<copy_only_type>{};
        v.emplace_back(0);
        v.emplace_back(1);
        v.emplace_back(2);
        v.emplace_back(3);
        v.emplace_back(4);
        v.emplace_back(5);
        auto even = mleivo::cu::filter(std::move(v), [](const auto& i) { return i.m_val % 2 == 0; });
        REQUIRE(even.size() == 3);
        for (int i = 0; i < even.size(); i++) {
            REQUIRE(i * 2 == even[i].m_val);
        }
    }
}

TEST_CASE( "test_split()", "container utils" ) {
    {
        auto v = std::vector<int>{0, -1, 1, -1, 2, -1, 3, -1, 4, -1, 5};
        auto out = mleivo::cu::split(v, -1);
        REQUIRE(out.size() == 6);
        for (int i = 0; i < out.size(); i++) {
            REQUIRE(out[i].size() == 1);
            REQUIRE(out[i].front() == i);
        }
    }

    {
        auto v = std::vector<int>{0, -1, 1, -1, 2, -1, 3, -1, 4, -1, 5};
        auto out = mleivo::cu::split(std::move(v), -1);
        REQUIRE(out.size() == 6);
        for (int i = 0; i < out.size(); i++) {
            REQUIRE(out[i].size() == 1);
            REQUIRE(out[i].front() == i);
        }
    }

    {
        auto out = mleivo::cu::split(std::vector<int>{0, -1, 1, -1, 2, -1, 3, -1, 4, -1, 5}, -1);
        REQUIRE(out.size() == 6);
        for (int i = 0; i < out.size(); i++) {
            REQUIRE(out[i].size() == 1);
            REQUIRE(out[i].front() == i);
        }
    }

    {
        auto v = std::vector<move_only_type>{};
        v.emplace_back(0);
        v.emplace_back(-1);
        v.emplace_back(1);
        v.emplace_back(-1);
        v.emplace_back(2);
        v.emplace_back(-1);
        v.emplace_back(3);
        v.emplace_back(-1);
        v.emplace_back(4);
        v.emplace_back(-1);
        v.emplace_back(5);
        auto out = mleivo::cu::split(std::move(v), move_only_type(-1));
        REQUIRE(out.size() == 6);
        for (int i = 0; i < out.size(); i++) {
            REQUIRE(out[i].size() == 1);
            REQUIRE(*out[i].front().m_val == i);
        }
    }

    {
        auto v = std::vector<copy_only_type>{};
        v.emplace_back(0);
        v.emplace_back(-1);
        v.emplace_back(1);
        v.emplace_back(-1);
        v.emplace_back(2);
        v.emplace_back(-1);
        v.emplace_back(3);
        v.emplace_back(-1);
        v.emplace_back(4);
        v.emplace_back(-1);
        v.emplace_back(5);
        auto out = mleivo::cu::split(std::move(v), copy_only_type(-1));
        REQUIRE(out.size() == 6);
        for (int i = 0; i < out.size(); i++) {
            REQUIRE(out[i].size() == 1);
            REQUIRE(out[i].front().m_val == i);
        }
    }
}

TEST_CASE( "test_to_std_vector()", "container utils" ) {
    {
        auto ms = std::multiset<int>{0, 1, 2, 3, 4, 5};
        auto v = mleivo::cu::to_std_vector(ms);
        static_assert(std::is_same_v<decltype(v), std::vector<mleivo::cu::value_type<decltype(ms)>>>);
        for (int i = 0; i < v.size(); i++) {
            REQUIRE(i == v[i]);
        }
    }
    {
        auto ms = std::multiset<int>{0, 1, 2, 3, 4, 5};
        auto v = mleivo::cu::to_std_vector(std::move(ms));
        static_assert(std::is_same_v<decltype(v), std::vector<mleivo::cu::value_type<decltype(ms)>>>);
        for (int i = 0; i < v.size(); i++) {
            REQUIRE(i == v[i]);
        }
    }
    {
        auto dq = std::deque<int>{0, 1, 2, 3, 4, 5};
        auto v = mleivo::cu::to_std_vector(dq);
        static_assert(std::is_same_v<decltype(v), std::vector<mleivo::cu::value_type<decltype(dq)>>>);
        for (int i = 0; i < v.size(); i++) {
            REQUIRE(i == v[i]);
        }
    }
    {
        auto dq = std::deque<int>{0, 1, 2, 3, 4, 5};
        auto v = mleivo::cu::to_std_vector(std::move(dq));
        static_assert(std::is_same_v<decltype(v), std::vector<mleivo::cu::value_type<decltype(dq)>>>);
        for (int i = 0; i < v.size(); i++) {
            REQUIRE(i == v[i]);
        }
    }
    {
        auto dq = std::deque<move_only_type>{};
        dq.emplace_back(0);
        dq.emplace_back(1);
        dq.emplace_back(2);
        dq.emplace_back(3);
        dq.emplace_back(4);
        dq.emplace_back(5);
        auto v = mleivo::cu::to_std_vector(std::move(dq));
        static_assert(std::is_same_v<decltype(v), std::vector<mleivo::cu::value_type<decltype(dq)>>>);
        for (int i = 0; i < v.size(); i++) {
            REQUIRE(i == *v[i].m_val);
        }
    }
    {
        auto dq = std::deque<copy_only_type>{};
        dq.emplace_back(0);
        dq.emplace_back(1);
        dq.emplace_back(2);
        dq.emplace_back(3);
        dq.emplace_back(4);
        dq.emplace_back(5);
        auto v = mleivo::cu::to_std_vector(std::move(dq));
        static_assert(std::is_same_v<decltype(v), std::vector<mleivo::cu::value_type<decltype(dq)>>>);
        for (int i = 0; i < v.size(); i++) {
            REQUIRE(i == v[i].m_val);
        }
    }
}

TEST_CASE( "test_compile_time_map()", "container utils" ) {
    using my_type_map = mleivo::type_traits::type_map<int, char, double, float>;
    static_assert(std::is_same_v<my_type_map::value<int>, char>);
    //    static_assert(std::is_same_v<my_type_map::value<bool>, char>);
}

TEST_CASE( "test_transform()", "container utils" ) {
    {
        auto in = std::vector<move_only_type>{};
        in.emplace_back(0);
        in.emplace_back(1);
        in.emplace_back(2);
        in.emplace_back(3);
        auto out = mleivo::cu::transform(std::move(in), [](move_only_type&& i) {
            return static_cast<double>(*i.m_val) * static_cast<double>(*i.m_val);
        });
        for (int i = 0; i < 4; i++) {
            REQUIRE(1.0 * i * i == out[i]);
        }
    }
    {
        auto in = std::vector<int>{};
        in.emplace_back(0);
        in.emplace_back(1);
        in.emplace_back(2);
        in.emplace_back(3);
        auto out = mleivo::cu::transform(std::move(in), [](int i) { return move_only_type(i); });
        for (int i = 0; i < 4; i++) {
            REQUIRE(i == *out[i].m_val);
        }
    }
    {
        auto in = std::vector<int>{};
        in.emplace_back(0);
        in.emplace_back(1);
        in.emplace_back(2);
        in.emplace_back(3);
        auto out = mleivo::cu::transform(std::move(in), [](int i) { return copy_only_type(i); });
        for (int i = 0; i < 4; i++) {
            REQUIRE(i == out[i].m_val);
        }
    }
    {
        auto in = std::vector<copy_only_type>{};
        in.emplace_back(0);
        in.emplace_back(1);
        in.emplace_back(2);
        in.emplace_back(3);
        auto out = mleivo::cu::transform(std::move(in), [](const copy_only_type& i) { return i.m_val; });
        for (int i = 0; i < 4; i++) {
            REQUIRE(i == out[i]);
        }
    }
}

TEST_CASE( "test_cont()", "container utils" ) {
    /* TODO
    Cont asd;
    for (auto& [index, ptr] : asd)
    {
        ptr = index * 2;
    }

    const Cont foo = asd;
    for (auto& [index, ptr] : foo)
    {
        std::cout << index << " " << ptr << std::endl;
    }
    */
}

TEST_CASE( "test_all_of()", "container utils" ) {
    std::set<int> s{1, 3, 5};
    REQUIRE(true == mleivo::cu::all_of(s, [](int i) { return i % 2; }));

    s.insert(2);
    REQUIRE(false == mleivo::cu::all_of(s, [](int i) { return i % 2; }));
}

TEST_CASE( "test_any_of()", "container utils" ) {
    std::set<int> s{1, 3, 5};
    REQUIRE(true == mleivo::cu::any_of(s, [](int i) { return i % 2; }));

    s.insert(2);
    REQUIRE(true == mleivo::cu::any_of(s, [](int i) { return i % 2; }));

    REQUIRE(false == mleivo::cu::any_of(s, [](int i) { return i > 5; }));
}

TEST_CASE( "test_remove_duplicates()", "container utils" ) {
    std::vector<int> v{0, 1, 0, 1, 2, 2, 2, 3, 2, 2};
    mleivo::cu::remove_duplicates(v);
    REQUIRE(4 == v.size());
    for (int i = 0; i < v.size(); ++i) {
        REQUIRE(i == v[i]);
    }
}

TEST_CASE( "test_index_of()", "container utils" ) {
    {
        auto v = std::vector<int>{0, 1, 2, 3, 4, 5};
        for (int i = 0; i < v.size(); ++i) {
            REQUIRE(i == mleivo::cu::index_of(v, i));
        }
        REQUIRE(6 == mleivo::cu::index_of(v, 6));
        REQUIRE(6 == mleivo::cu::index_of(v, -42));
    }
    {
        auto v = std::vector<int>{0, 1, 2, 3, 4, 5};
        for (int i = 0; i < v.size(); ++i) {
            REQUIRE(i == mleivo::cu::index_of(v, [i](int j) { return i == j; }));
        }
    }
}

TEST_CASE( "test_irange()", "container utils") {
    {
        auto a = int{0};
        auto b = int{10};
        auto step = int{3};
        auto v = mleivo::cu::irange(a, b, step);
        auto ans = a;
        for (auto i : v)
        {
            REQUIRE(ans == i);
            ans += step;
        }
    }
    {
        auto a = int{10};
        auto b = int{0};
        auto step = int{-1};
        auto v = mleivo::cu::irange(a, b, step);
        auto ans = a;
        for (auto i : v)
        {
            REQUIRE(ans == i);
            ans += step;
        }
    }
    {
        auto a = int{10};
        auto b = int{0};
        auto step = int{-3};
        auto v = mleivo::cu::irange(a, b, step);
        auto ans = a;
        for (auto i : v)
        {
            REQUIRE(ans == i);
            ans += step;
        }
    }
    {
        auto a = int{10};
        auto b = int{-10};
        auto step = int{-3};
        auto v = mleivo::cu::irange(a, b, step);
        auto ans = a;
        for (auto i : v)
        {
            REQUIRE(ans == i);
            ans += step;
        }
    }
}
