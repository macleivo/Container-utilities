/*
 * Copyright 2022 Marcus Leivo
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include <bits/stdc++.h>

#include "containerutils.h"
#include "enumerate.h"

auto g_ret_val = int{0};

#define COMPARE(a, b)                                                                                                  \
    if ((a) != (b)) {                                                                                                  \
        std::cout << "FAIL: " << (a) << " != " << (b) << "\n";                                                         \
        g_ret_val = 1;                                                                                                 \
    }

template <typename ContainerT1, typename ContainerT2> bool cmp(const ContainerT1& c1, const ContainerT2& c2) {
    if (c1.size() != c2.size())
        return false;
    for (size_t i = 0; i < c1.size(); ++i) {
        if (c1[i] != c2[i])
            return false;
    }
    return true;
}

struct move_only_type {
    explicit move_only_type(int val) : m_val(std::make_unique<int>(val)) {
    }
    ~move_only_type() = default;
    move_only_type(const move_only_type&) = delete;
    move_only_type& operator=(const move_only_type&) = delete;
    move_only_type(move_only_type&&) = default;
    move_only_type& operator=(move_only_type&&) = default;

    std::unique_ptr<int> m_val;

    friend bool operator<(const move_only_type& lhs, const move_only_type& rhs) {
        return *lhs.m_val < *rhs.m_val;
    }

    friend bool operator==(const move_only_type& lhs, const move_only_type& rhs) {
        return *lhs.m_val == *rhs.m_val;
    }
};

struct copy_only_type {
    explicit copy_only_type(int val) : m_val(val) {
    }
    ~copy_only_type() = default;
    copy_only_type(const copy_only_type&) = default;
    copy_only_type& operator=(const copy_only_type&) = default;
    copy_only_type(copy_only_type&&) = delete;
    copy_only_type& operator=(copy_only_type&&) = delete;

    int m_val;

    friend bool operator<(const copy_only_type& lhs, const copy_only_type& rhs) {
        return lhs.m_val < rhs.m_val;
    }

    friend bool operator==(const copy_only_type& lhs, const copy_only_type& rhs) {
        return lhs.m_val == rhs.m_val;
    }

    friend void swap(copy_only_type& lhs, copy_only_type& rhs) {
        using std::swap;
        swap(lhs.m_val, rhs.m_val);
    }
};

struct Base {
    int operator()() const {
        return 0xB;
    }
};

struct Derived : public Base {
    int operator()() const {
        return 0xD;
    }
};

void test_contains() {
    struct wrapper : public std::vector<int> {
        bool contains(int i) const {
            return std::find(std::cbegin(*this), std::cend(*this), i) != std::cend(*this);
        }
    };

    {
        auto s = std::set<int>{0, 1, 2, 3};
        COMPARE(true, mleivo::cu::contains(s, 0));
        COMPARE(false, mleivo::cu::contains(s, -1));
    }

    {
        auto w = wrapper{};
        w.emplace_back(0);
        w.emplace_back(1);
        w.emplace_back(2);
        COMPARE(true, mleivo::cu::contains(w, 0));
        COMPARE(false, mleivo::cu::contains(w, -1));
    }

    {
        auto v = std::vector<int>{0, 1, 2, 3, 4};
        COMPARE(true, mleivo::cu::contains(v, 0));
        COMPARE(false, mleivo::cu::contains(v, -1));
    }

    {
        std::vector<std::unique_ptr<int>> v;
        v.emplace_back(std::make_unique<int>(0));
        v.emplace_back(std::make_unique<int>(1));
        v.emplace_back(std::make_unique<int>(2));
        v.emplace_back(std::make_unique<int>(3));

        COMPARE(true, mleivo::cu::contains(v, v.front()));
        COMPARE(false, mleivo::cu::contains(v, nullptr));
        COMPARE(false, mleivo::cu::contains(v, std::make_unique<int>(0)));
    }
}

void test_merge() {
    {
        std::vector<int> v1{0, 1};
        std::set<int> v2{2, 3};
        std::deque<int> v3{4, 5};

        const auto merged = mleivo::cu::merge(v1, v2, v3);
        COMPARE(merged.size(), 6);
        for (int i = 0; i < merged.size(); i++) {
            COMPARE(i, merged[i]);
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
        COMPARE(merged.size(), 6);
        for (int i = 0; i < merged.size(); i++) {
            COMPARE(i, *merged[i].m_val);
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
        COMPARE(merged.size(), 6);
        for (int i = 0; i < merged.size(); i++) {
            COMPARE(i, merged[i].m_val);
        }
    }
}

void test_filter() {
    {
        auto ms = std::multiset<int>{0, 1, 2, 3, 4, 5};
        auto even = mleivo::cu::filter(ms, [](const auto& i) { return i % 2 == 0; });
        COMPARE(even.size(), 3);
        for (int i = 0; i < even.size(); i++) {
            COMPARE(i * 2, even[i]);
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
        COMPARE(even.size(), 3);
        std::sort(even.begin(), even.end());
        for (int i = 0; i < even.size(); i++) {
            COMPARE(i * 2, *even[i].m_val);
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
        COMPARE(even.size(), 3);
        for (int i = 0; i < even.size(); i++) {
            COMPARE(i * 2, even[i].m_val);
        }
    }
}

void test_split() {
    {
        auto v = std::vector<int>{0, -1, 1, -1, 2, -1, 3, -1, 4, -1, 5};
        auto out = mleivo::cu::split(v, -1);
        COMPARE(out.size(), 6);
        for (int i = 0; i < out.size(); i++) {
            COMPARE(out[i].size(), 1);
            COMPARE(out[i].front(), i);
        }
    }

    {
        auto v = std::vector<int>{0, -1, 1, -1, 2, -1, 3, -1, 4, -1, 5};
        auto out = mleivo::cu::split(std::move(v), -1);
        COMPARE(out.size(), 6);
        for (int i = 0; i < out.size(); i++) {
            COMPARE(out[i].size(), 1);
            COMPARE(out[i].front(), i);
        }
    }

    {
        auto out = mleivo::cu::split(std::vector<int>{0, -1, 1, -1, 2, -1, 3, -1, 4, -1, 5}, -1);
        COMPARE(out.size(), 6);
        for (int i = 0; i < out.size(); i++) {
            COMPARE(out[i].size(), 1);
            COMPARE(out[i].front(), i);
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
        COMPARE(out.size(), 6);
        for (int i = 0; i < out.size(); i++) {
            COMPARE(out[i].size(), 1);
            COMPARE(*out[i].front().m_val, i);
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
        COMPARE(out.size(), 6);
        for (int i = 0; i < out.size(); i++) {
            COMPARE(out[i].size(), 1);
            COMPARE(out[i].front().m_val, i);
        }
    }
}

void test_to_std_vector() {
    {
        auto ms = std::multiset<int>{0, 1, 2, 3, 4, 5};
        auto v = mleivo::cu::to_std_vector(ms);
        static_assert(std::is_same_v<decltype(v), std::vector<mleivo::cu::value_type<decltype(ms)>>>);
        for (int i = 0; i < v.size(); i++) {
            COMPARE(i, v[i]);
        }
    }
    {
        auto ms = std::multiset<int>{0, 1, 2, 3, 4, 5};
        auto v = mleivo::cu::to_std_vector(std::move(ms));
        static_assert(std::is_same_v<decltype(v), std::vector<mleivo::cu::value_type<decltype(ms)>>>);
        for (int i = 0; i < v.size(); i++) {
            COMPARE(i, v[i]);
        }
    }
    {
        auto dq = std::deque<int>{0, 1, 2, 3, 4, 5};
        auto v = mleivo::cu::to_std_vector(dq);
        static_assert(std::is_same_v<decltype(v), std::vector<mleivo::cu::value_type<decltype(dq)>>>);
        for (int i = 0; i < v.size(); i++) {
            COMPARE(i, v[i]);
        }
    }
    {
        auto dq = std::deque<int>{0, 1, 2, 3, 4, 5};
        auto v = mleivo::cu::to_std_vector(std::move(dq));
        static_assert(std::is_same_v<decltype(v), std::vector<mleivo::cu::value_type<decltype(dq)>>>);
        for (int i = 0; i < v.size(); i++) {
            COMPARE(i, v[i]);
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
            COMPARE(i, *v[i].m_val);
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
            COMPARE(i, v[i].m_val);
        }
    }
}

void test_compile_time_map() {
    using my_type_map = mleivo::type_traits::type_map<int, char, double, float>;
    static_assert(std::is_same_v<my_type_map::value<int>, char>);
    //    static_assert(std::is_same_v<my_type_map::value<bool>, char>);
}

void test_transform() {
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
            COMPARE(1.0 * i * i, out[i]);
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
            COMPARE(i, *out[i].m_val);
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
            COMPARE(i, out[i].m_val);
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
            COMPARE(i, out[i]);
        }
    }
}

void test_cont() {
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

void test_all_of() {
    std::set<int> s{1, 3, 5};
    COMPARE(true, mleivo::cu::all_of(s, [](int i) { return i % 2; }));

    s.insert(2);
    COMPARE(false, mleivo::cu::all_of(s, [](int i) { return i % 2; }));
}

void test_any_of() {
    std::set<int> s{1, 3, 5};
    COMPARE(true, mleivo::cu::any_of(s, [](int i) { return i % 2; }));

    s.insert(2);
    COMPARE(true, mleivo::cu::any_of(s, [](int i) { return i % 2; }));

    COMPARE(false, mleivo::cu::any_of(s, [](int i) { return i > 5; }));
}

void test_remove_duplicates() {
    std::vector<int> v{0, 1, 0, 1, 2, 2, 2, 3, 2, 2};
    mleivo::cu::remove_duplicates(v);
    COMPARE(4, v.size());
    for (int i = 0; i < v.size(); ++i) {
        COMPARE(i, v[i]);
    }
}

template <typename...> class TD;

void test_remove_all() {
    {
        std::vector<int> v{1, 2, 1, 2, 1};
        mleivo::cu::remove_all(v, 1);
        COMPARE(true, cmp(std::vector<int>{2, 2}, v));
        mleivo::cu::remove_all(v, 2);
        COMPARE(0, v.size());
    }
    {
        std::vector<int> v{0, 1, 2, 3, 4, 5};
        mleivo::cu::remove_all(v, [](int i) { return i % 2 == 0; });
        COMPARE(true, cmp(std::vector<int>{1, 3, 5}, v));
    }
}

void test_move_to_index() {
    {
        std::vector<int> v{3, 1, 2, 0};
        mleivo::cu::move_to_index(v, 3, 0);
        COMPARE(true, cmp(std::vector<int>{0, 3, 1, 2}, v));

        mleivo::cu::move_to_index(v, 1, 3);
        COMPARE(true, cmp(std::vector<int>{0, 1, 2, 3}, v));

        mleivo::cu::move_to_index(v, 1, 1);
        COMPARE(true, cmp(std::vector<int>{0, 1, 2, 3}, v));
    }
}

void test_pop_front() {
    {
        std::vector<int> v{0, 1, 2};
        mleivo::cu::pop_front(v);
        COMPARE(1, v.front())
        COMPARE(2, v.size())

        mleivo::cu::pop_front(v);
        COMPARE(1, v.size())
        COMPARE(2, v.front())

        mleivo::cu::pop_front(v);
        COMPARE(0, v.size())
    }
}

void test_static_cast_all() {
    {
        auto v = std::vector<double>{0, 1, 2};
        auto ints = mleivo::cu::static_cast_all<int>(v);
        static_assert(std::is_same_v<decltype(ints), std::vector<int>>);
        COMPARE(3, ints.size())
        for (int i = 0; i < ints.size(); ++i) {
            COMPARE(i, ints[i]);
        }
    }
    {
        auto v = std::deque<double>{0, 1, 2};
        auto ints = mleivo::cu::static_cast_all<int>(v);
        static_assert(std::is_same_v<decltype(ints), std::deque<int>>);
        COMPARE(3, ints.size())
        mleivo::cu::contains(ints, 0);
        mleivo::cu::contains(ints, 1);
        mleivo::cu::contains(ints, 2);
    }
    {
        double v[3] = {0, 1, 2};
        auto ints = mleivo::cu::static_cast_all<int>(v);
        static_assert(std::is_same_v<decltype(ints), std::array<int, 3>>);
        COMPARE(3, ints.size())
        for (int i = 0; i < ints.size(); ++i) {
            COMPARE(i, ints[i]);
        }
    }
    {
        std::array<double, 3> v = {0, 1, 2};
        auto ints = mleivo::cu::static_cast_all<int>(v);
        static_assert(std::is_same_v<decltype(ints), std::array<int, 3>>);
        COMPARE(3, ints.size())
        for (int i = 0; i < ints.size(); ++i) {
            COMPARE(i, ints[i]);
        }
    }

    {
        auto as_base = std::vector<Base*>{};
        as_base.emplace_back(new Derived);
        as_base.emplace_back(new Derived);
        as_base.emplace_back(new Derived);

        for (const auto* b : as_base) {
            COMPARE((*b)(), 0xB);
        }

        auto as_der = mleivo::cu::static_cast_all<Derived*>(as_base);
        for (const auto* d : as_der) {
            COMPARE((*d)(), 0xD);
        }
    }
}

void test_index_of() {
    {
        auto v = std::vector<int>{0, 1, 2, 3, 4, 5};
        for (int i = 0; i < v.size(); ++i) {
            COMPARE(i, mleivo::cu::index_of(v, i));
        }
        COMPARE(6, mleivo::cu::index_of(v, 6));
        COMPARE(6, mleivo::cu::index_of(v, -42));
    }
    {
        auto v = std::vector<int>{0, 1, 2, 3, 4, 5};
        for (int i = 0; i < v.size(); ++i) {
            COMPARE(i, mleivo::cu::index_of(v, [i](int j) { return i == j; }));
        }
    }
}

int main() {
    test_merge();
    test_filter();
    test_split();
    test_to_std_vector();
    test_contains();
    test_transform();
    test_compile_time_map();
    test_cont();
    test_all_of();
    test_any_of();
    test_remove_duplicates();
    test_remove_all();
    test_move_to_index();
    test_pop_front();
    test_static_cast_all();
    test_index_of();
    return g_ret_val;
}
