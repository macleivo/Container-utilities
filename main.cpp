#include <bits/stdc++.h>

#include "containerutils.h"

auto g_ret_val = int { 0 };

#define COMPARE(a, b)                                                                                                  \
    if ((a) != (b))                                                                                                    \
    {                                                                                                                  \
        std::cout << "FAIL: " << (a) << " != " << (b) << "\n";                                                         \
        g_ret_val = 1;                                                                                                 \
    }

struct move_only_type
{
    explicit move_only_type(int val) : m_val(val)
    {
    }
    ~move_only_type() = default;
    move_only_type(const move_only_type&) = delete;
    move_only_type& operator=(const move_only_type&) = delete;
    move_only_type(move_only_type&&) = default;
    move_only_type& operator=(move_only_type&&) = default;

    int m_val;

    friend bool operator<(const move_only_type& lhs, const move_only_type& rhs)
    {
        return lhs.m_val < rhs.m_val;
    }
};

struct copy_only_type
{
    explicit copy_only_type(int val) : m_val(val)
    {
    }
    ~copy_only_type() = default;
    copy_only_type(const copy_only_type&) = default;
    copy_only_type& operator=(const copy_only_type&) = default;
    copy_only_type(copy_only_type&&) = delete;
    copy_only_type& operator=(copy_only_type&&) = delete;

    int m_val;

    friend bool operator<(const copy_only_type& lhs, const copy_only_type& rhs)
    {
        return lhs.m_val < rhs.m_val;
    }

    friend void swap(copy_only_type& lhs, copy_only_type& rhs)
    {
        using std::swap;
        swap(lhs.m_val, rhs.m_val);
    }
};

void testMerge()
{
    {
        std::vector<int> v1 { 0, 1 };
        std::set<int> v2 { 2, 3 };
        std::deque<int> v3 { 4, 5 };

        const auto merged = cu::merge(v1, v2, v3);
        COMPARE(merged.size(), 6);
        for (int i = 0; i < merged.size(); i++)
        {
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

        const auto merged = cu::merge(std::move(v1), std::move(v2), std::move(v3));
        COMPARE(merged.size(), 6);
        for (int i = 0; i < merged.size(); i++)
        {
            COMPARE(i, merged[i].m_val);
        }
    }
}

void testFilter()
{
    {
        auto ms = std::multiset<int> { 0, 1, 2, 3, 4, 5 };
        auto even = cu::filter(ms, [](const auto& i) { return i % 2 == 0; });
        COMPARE(even.size(), 3);
        for (int i = 0; i < 3; i++)
        {
            COMPARE(i * 2, even[i]);
        }
    }

    {
        auto v = std::vector<move_only_type> {};
        v.emplace_back(5);
        v.emplace_back(4);
        v.emplace_back(3);
        v.emplace_back(2);
        v.emplace_back(1);
        v.emplace_back(0);
        auto even = cu::filter(std::move(v), [](const auto& i) { return i.m_val % 2 == 0; });
        COMPARE(even.size(), 3);
        std::sort(even.begin(), even.end());
        for (int i = 0; i < 3; i++)
        {
            COMPARE(i * 2, even[i].m_val);
        }
    }

    {
        auto v = std::vector<copy_only_type> {};
        v.emplace_back(0);
        v.emplace_back(1);
        v.emplace_back(2);
        v.emplace_back(3);
        v.emplace_back(4);
        v.emplace_back(5);
        auto even = cu::filter(std::move(v), [](const auto& i) { return i.m_val % 2 == 0; });
        COMPARE(even.size(), 3);
        for (int i = 0; i < 3; i++)
        {
            COMPARE(i * 2, even[i].m_val);
        }
    }
}

int main()
{
    testMerge();
    testFilter();
    return g_ret_val;
}
