#pragma once

#include "type_traits.h"

namespace cu
{
using type_traits::value_type;

// filter
template<typename ContainerT, typename Filter>
std::vector<value_type<ContainerT>> filter(ContainerT&& c, Filter&& f)
{
    std::vector<value_type<decltype(c)>> v;
    if constexpr (std::is_rvalue_reference_v<decltype(c)> && std::is_move_constructible_v<value_type<decltype(c)>>)
    {
        std::copy_if(std::make_move_iterator(std::begin(c)), std::make_move_iterator(std::end(c)),
                     std::back_inserter(v), std::forward<Filter>(f));
    }
    else
    {
        std::copy_if(std::begin(c), std::end(c), std::back_inserter(v), std::forward<Filter>(f));
    }
    return v;
}

// merge
template<typename ContainerT>
std::vector<value_type<ContainerT>> merge(ContainerT&& c)
{
    std::vector<value_type<ContainerT>> out;
    if constexpr (std::is_rvalue_reference_v<decltype(c)> && std::is_move_constructible_v<value_type<decltype(c)>>)
    {
        out.insert(out.end(), std::make_move_iterator(std::begin(c)), std::make_move_iterator(std::end(c)));
    }
    else
    {
        std::copy(std::begin(c), std::end(c), std::back_inserter(out));
    }
    return out;
}

template<typename ContainerT1, typename... ContainerT2ToN>
std::enable_if_t<type_traits::value_types_equal_v<ContainerT1, ContainerT2ToN...>, std::vector<value_type<ContainerT1>>>
merge(ContainerT1&& c1, ContainerT2ToN&&... c2ToN)
{
    auto out = merge(std::forward<ContainerT1>(c1));
    auto mergedTail = merge(std::forward<ContainerT2ToN>(c2ToN)...);
    if constexpr (std::is_move_constructible_v<value_type<decltype(mergedTail)>>)
    {
        out.insert(out.end(), std::make_move_iterator(std::begin(mergedTail)),
                   std::make_move_iterator(std::end(mergedTail)));
    }
    else
    {
        std::copy(std::begin(mergedTail), std::end(mergedTail), std::back_inserter(out));
    }

    return out;
}

}
