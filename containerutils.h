#pragma once

#include "type_traits.h"

namespace cu
{
using type_traits::value_type;

template<typename ContainerT>
std::vector<value_type<ContainerT>> to_std_vector(ContainerT&& c);

// contains
template<typename ContainerT>
bool contains(ContainerT&& c, const value_type<ContainerT>& value)
{
    if constexpr (type_traits::has_method_contains_v<ContainerT>)
    {
        return c.contains(value);
    }
    else if constexpr (type_traits::has_method_count_v<ContainerT>)
    {
        return c.count(value) != 0;
    }
    else
    {
        return std::find(std::begin(c), std::end(c), value) != std::end(c);
    }
}

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
    return to_std_vector(std::forward<ContainerT>(c));
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

template<typename ContainerT>
std::vector<std::decay_t<ContainerT>> split(ContainerT&& c, const value_type<ContainerT>& separator)
{
    auto out = std::vector<std::decay_t<ContainerT>> {};
    out.emplace_back();
    for (auto&& value : c)
    {
        if (value == separator)
        {
            out.emplace_back();
            continue;
        }

        if constexpr (std::is_rvalue_reference_v<decltype(c)> && std::is_move_constructible_v<value_type<decltype(c)>>)
        {
            out.back().emplace_back(std::move(value));
        }
        else
        {
            out.back().push_back(value);
        }
    }

    return out;
}

// to_std_vector
template<typename ContainerT>
std::vector<value_type<ContainerT>> to_std_vector(ContainerT&& c)
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
}
