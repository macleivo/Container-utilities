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
#pragma once

#include "type_traits.h"

#include <algorithm>
#include <vector>

namespace mleivo::cu
{
using ::mleivo::type_traits::value_type;

template<typename ContainerT>
std::vector<value_type<ContainerT>> to_std_vector(ContainerT&& c);

// all_of
template<typename ContainerT, typename UnaryPredicate>
bool all_of(const ContainerT& container, UnaryPredicate&& predicate)
{
    using std::begin;
    using std::end;
    return std::all_of(begin(container), end(container), predicate);
}
// any_of
template<typename ContainerT, typename UnaryPredicate>
bool any_of(const ContainerT& container, UnaryPredicate&& predicate)
{
    using std::begin;
    using std::end;
    return std::any_of(begin(container), end(container), predicate);
}

// contains
template<typename ContainerT, typename ValueT>
bool contains(const ContainerT& c, const ValueT& value)
{
    if constexpr (mleivo::type_traits::has_method_contains_v<ContainerT>)
    {
        return c.contains(value);
    }
    else if constexpr (mleivo::type_traits::has_method_count_v<ContainerT>)
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
std::enable_if_t<mleivo::type_traits::value_types_equal_v<ContainerT1, ContainerT2ToN...>, std::vector<value_type<ContainerT1>>>
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

template<typename ContainerT, typename EqualityCmp>
void remove_duplicates(ContainerT& container, const EqualityCmp& cmp)
{
    using std::size;
    auto n = size(container);
    for (size_t i = 0; i < n; ++i)
    {
        const auto& item = container[i];
        for (size_t j = n - 1; j > i; --j)
        {
            if (cmp(item, container[j]))
            {
                container.erase(container.begin() + j);
                --n;
            }
        }
    }
}

template<typename ContainerT>
void remove_duplicates(ContainerT& container)
{
    remove_duplicates(container, std::equal_to{});
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

// transform
template<typename ContainerT, typename TransformerT>
auto transform(ContainerT&& c, TransformerT&& t)
{
    using t_ret_val = decltype(std::declval<TransformerT>()(std::declval<value_type<ContainerT>>()));
    std::conditional_t<std::is_same_v<value_type<ContainerT>, t_ret_val>,
                       std::decay_t<ContainerT>,
                       std::vector<t_ret_val>>
      out;
    if constexpr (std::is_rvalue_reference_v<decltype(c)> && std::is_move_constructible_v<value_type<decltype(out)>>)
    {
        std::transform(std::make_move_iterator(std::begin(c)),
                       std::make_move_iterator(std::end(c)),
                       std::back_inserter(out),
                       std::forward<TransformerT>(t));
    }
    else
    {
        for (auto i : c)
        {
            auto tmp = t(i);
            out.push_back(tmp);
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
