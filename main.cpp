#include <bits/stdc++.h>

#include "containerutils.h"

#define COMPARE(a, b)                                                                                                  \
    if ((a) != (b))                                                                                                    \
        std::cout << "FAIL: " << (a) << " != " << (b) << "\n";

void testMerge()
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

int main()
{
    testMerge();
    return 0;
}
