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
#include <cassert>
#include <vector>

namespace mleivo::cu {
using ::mleivo::type_traits::value_type;

namespace detail {
template <typename To, typename From> auto static_cast_all_default_imp(const From& from) {
    using std::cbegin;
    using std::cend;
    To out;
    std::transform(cbegin(from), cend(from), std::back_inserter(out),
                   [](auto& e) { return static_cast<value_type<To>>(e); });
    return out;
}
} // namespace detail

template <typename ContainerT> std::vector<value_type<ContainerT>> to_std_vector(ContainerT&& c);

// all_of
template <typename ContainerT, typename UnaryPredicate>
bool all_of(const ContainerT& container, UnaryPredicate&& predicate) {
    using std::begin;
    using std::end;
    return std::all_of(begin(container), end(container), predicate);
}

// any_of
template <typename ContainerT, typename UnaryPredicate>
bool any_of(const ContainerT& container, UnaryPredicate&& predicate) {
    using std::begin;
    using std::end;
    return std::any_of(begin(container), end(container), predicate);
}

// static_cast_all
template <typename T, typename ValueT, template <typename...> typename ContainerT, typename... ContainerTArgs>
auto static_cast_all(const ContainerT<ValueT, ContainerTArgs...>& container) {
    return detail::static_cast_all_default_imp<ContainerT<T>>(container);
}

template <typename OutT, typename InT, size_t N> auto static_cast_all(const InT (&container)[N]) {
    using std::back_inserter;
    using std::begin;
    using std::end;
    auto out = std::array<OutT, N>();
    std::copy(begin(container), end(container), begin(out));
    return out;
}

template <typename T, typename FromT, size_t N, template <typename, size_t> typename ContainerT>
auto static_cast_all(const ContainerT<FromT, N>& container) {
    using std::back_inserter;
    using std::begin;
    using std::end;
    auto out = ContainerT<T, N>();
    std::copy(begin(container), end(container), begin(out));
    return out;
}

template <typename T, typename ContainerT> // fall back to returning an std::vector
auto static_cast_all(const ContainerT& container) {
    return detail::static_cast_all_default_imp<std::vector<T>>(container);
}

// contains
template <typename ContainerT, typename ValueT> bool contains(const ContainerT& c, const ValueT& value) {
    if constexpr (mleivo::type_traits::has_method_contains_v<ContainerT>) {
        return c.contains(value);
    } else if constexpr (mleivo::type_traits::has_method_count_v<ContainerT>) {
        return c.count(value) != 0;
    } else {
        return std::find(std::begin(c), std::end(c), value) != std::end(c);
    }
}

// filter
template <typename ContainerT, typename Filter> std::vector<value_type<ContainerT>> filter(ContainerT&& c, Filter&& f) {
    std::vector<value_type<decltype(c)>> v;
    if constexpr (std::is_rvalue_reference_v<decltype(c)> && std::is_move_constructible_v<value_type<decltype(c)>>) {
        std::copy_if(std::make_move_iterator(std::begin(c)), std::make_move_iterator(std::end(c)),
                     std::back_inserter(v), std::forward<Filter>(f));
    } else {
        std::copy_if(std::begin(c), std::end(c), std::back_inserter(v), std::forward<Filter>(f));
    }
    return v;
}

// merge
template <typename ContainerT> std::vector<value_type<ContainerT>> merge(ContainerT&& c) {
    return to_std_vector(std::forward<ContainerT>(c));
}

template <typename ContainerT1, typename... ContainerT2ToN>
std::enable_if_t<mleivo::type_traits::value_types_equal_v<ContainerT1, ContainerT2ToN...>,
                 std::vector<value_type<ContainerT1>>>
merge(ContainerT1&& c1, ContainerT2ToN&&... c2ToN) {
    auto out = merge(std::forward<ContainerT1>(c1));
    auto mergedTail = merge(std::forward<ContainerT2ToN>(c2ToN)...);
    if constexpr (std::is_move_constructible_v<value_type<decltype(mergedTail)>>) {
        out.insert(out.end(), std::make_move_iterator(std::begin(mergedTail)),
                   std::make_move_iterator(std::end(mergedTail)));
    } else {
        std::copy(std::begin(mergedTail), std::end(mergedTail), std::back_inserter(out));
    }

    return out;
}

// move_to_index
template <typename ContainerT>
void move_to_index(ContainerT& container, typename std::decay_t<ContainerT>::difference_type oldIndex,
                   typename std::decay_t<ContainerT>::difference_type newIndex) {
    using std::begin;
    using std::rend;
    if (oldIndex > newIndex) {
        std::rotate(rend(container) - oldIndex - 1, rend(container) - oldIndex, rend(container) - newIndex);
        return;
    }

    std::rotate(begin(container) + oldIndex, begin(container) + oldIndex + 1, begin(container) + newIndex + 1);
}

// index_of
template <typename ContainerT, typename T> auto index_of(const ContainerT& container, T&& predOrItem) {
    using std::cbegin;
    using std::cend;
    if constexpr (std::is_same_v<value_type<ContainerT>, std::decay_t<T>>)
        return std::distance(cbegin(container), std::find(cbegin(container), cend(container), predOrItem));
    else
        return std::distance(cbegin(container),
                             std::find_if(cbegin(container), cend(container), std::forward<T>(predOrItem)));
}

// pop_front
template <typename ContainerT> void pop_front(ContainerT& v) {
    using std::begin;
    assert(!v.empty());
    v.erase(begin(v));
}

// remove_all
template <typename ContainerT, typename T> void remove_all(ContainerT& container, T&& predOrItem) {
    using std::begin;
    using std::end;
    if constexpr (mleivo::type_traits::is_unary_predicate_v<decltype(predOrItem), value_type<ContainerT>>)
        container.erase(std::remove_if(begin(container), end(container), std::forward<T>(predOrItem)), end(container));
    else
        container.erase(std::remove(begin(container), end(container), std::forward<T>(predOrItem)), end(container));
}

// remove_duplicates
template <typename ContainerT, typename EqualityCmp>
void remove_duplicates(ContainerT& container, const EqualityCmp& cmp) {
    using std::size;
    auto n = size(container);
    for (size_t i = 0; i < n; ++i) {
        const auto& item = container[i];
        for (size_t j = n - 1; j > i; --j) {
            if (cmp(item, container[j])) {
                container.erase(container.begin() + j);
                --n;
            }
        }
    }
}

template <typename ContainerT> void remove_duplicates(ContainerT& container) {
    remove_duplicates(container, std::equal_to{});
}

template <typename ContainerT>
std::vector<std::decay_t<ContainerT>> split(ContainerT&& c, const value_type<ContainerT>& separator) {
    auto out = std::vector<std::decay_t<ContainerT>>{};
    out.emplace_back();
    for (auto&& value : c) {
        if (value == separator) {
            out.emplace_back();
            continue;
        }

        if constexpr (std::is_rvalue_reference_v<decltype(c)>
                      && std::is_move_constructible_v<value_type<decltype(c)>>) {
            out.back().emplace_back(std::move(value));
        } else {
            out.back().push_back(value);
        }
    }

    return out;
}

// transform
template <typename ContainerT, typename TransformerT> auto transform(ContainerT&& c, TransformerT&& t) {
    using t_ret_val = decltype(std::declval<TransformerT>()(std::declval<value_type<ContainerT>>()));
    std::conditional_t<std::is_same_v<value_type<ContainerT>, t_ret_val>, std::decay_t<ContainerT>,
                       std::vector<t_ret_val>>
        out;
    if constexpr (std::is_rvalue_reference_v<decltype(c)> && std::is_move_constructible_v<value_type<decltype(out)>>) {
        std::transform(std::make_move_iterator(std::begin(c)), std::make_move_iterator(std::end(c)),
                       std::back_inserter(out), std::forward<TransformerT>(t));
    } else {
        for (auto i : c) {
            auto tmp = t(i);
            out.push_back(tmp);
        }
    }
    return out;
}

// to_std_vector
template <typename ContainerT> std::vector<value_type<ContainerT>> to_std_vector(ContainerT&& c) {
    std::vector<value_type<ContainerT>> out;
    if constexpr (std::is_rvalue_reference_v<decltype(c)> && std::is_move_constructible_v<value_type<decltype(c)>>) {
        out.insert(out.end(), std::make_move_iterator(std::begin(c)), std::make_move_iterator(std::end(c)));
    } else {
        std::copy(std::begin(c), std::end(c), std::back_inserter(out));
    }
    return out;
}
} // namespace mleivo::cu
