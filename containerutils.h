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
#include <array>
#include <cassert>
#include <vector>

namespace mleivo::cu {
using ::mleivo::type_traits::value_type;

namespace detail {
struct empty_struct {};

template <typename To, typename From>
auto static_cast_all_default_imp(const From& from) {
    using std::cbegin;
    using std::cend;
    To out;
    std::transform(cbegin(from), cend(from), std::back_inserter(out),
                   [](auto& e) { return static_cast<value_type<To>>(e); });
    return out;
}

template <typename It>
struct iter_value_type {
    size_t m_index;
    It m_iter;
};

template <typename It>
struct iter {
    iter(size_t i, It it) : m_value{i, std::move(it)} {
    }

    auto& operator++() {
        ++m_value.m_index;
        ++m_value.m_iter;
        return *this;
    }

    auto& operator*() {
        return m_value;
    }

    bool operator!=(const iter& rhs) {
        return m_value.m_iter != rhs.m_value.m_iter || m_value.m_index != rhs.m_value.m_index;
    }

    iter_value_type<It> m_value;
};

template <typename ContainerT, typename It>
struct enumerate_struct
    : std::conditional_t<std::is_rvalue_reference_v<ContainerT>, std::remove_reference_t<ContainerT>, empty_struct> {
    enumerate_struct(It begin, It end, size_t size) : m_begin(std::move(begin)), m_end(std::move(end)), m_size(size) {
    }

    template <typename X = ContainerT, typename = std::enable_if_t<std::is_rvalue_reference_v<X>>>
    enumerate_struct(ContainerT container)
        : std::remove_reference_t<ContainerT>(std::move(container)),
          m_begin(std::begin(static_cast<std::remove_reference_t<ContainerT>&>(*this))),
          m_end(std::end(static_cast<std::remove_reference_t<ContainerT>&>(*this))),
          m_size(std::size(static_cast<std::remove_reference_t<ContainerT>&>(*this))) {
    }

    auto begin() {
        return iter<decltype(m_begin)>{size_t{0}, m_begin};
    }
    auto end() {
        return iter<decltype(m_end)>{size_t{m_size}, m_end};
    }

    It m_begin;
    It m_end;
    size_t m_size;
};

template <int Index, typename It>
auto get(iter_value_type<It>&& it) {
    if constexpr (Index == 0)
        return it.m_index;
    else if constexpr (Index == 1)
        return *it.m_iter;
}

template <int Index, typename It>
auto& get(iter_value_type<It>& it) {
    if constexpr (Index == 0)
        return it.m_index;
    else if constexpr (Index == 1)
        return *it.m_iter;
}

template <int Index, typename It>
const auto& get(const iter_value_type<It>& it) {
    if constexpr (Index == 0)
        return it.m_index;
    else if constexpr (Index == 1)
        return *it.m_iter;
}

template<typename T>
constexpr auto begin_(T&& container)
{
    using std::begin;
    return begin(container);
}

template<typename T>
constexpr auto end_(T&& container)
{
    using std::end;
    return end(container);
}

template<typename T>
constexpr auto size_(T&& container)
{
    using std::size;
    return size(container);
}
} // namespace detail

template <typename ContainerT>
struct Enumerate
{
    auto begin() {
        auto it = detail::begin_(m_container);
        return detail::iter<decltype(it)>{size_t{0}, it};
    }
    auto end() {
        auto it = detail::end_(m_container);
        return detail::iter<decltype(it)>{size_t{m_size}, it};
    }

    using T = std::conditional_t<std::is_rvalue_reference_v<ContainerT&&>, std::remove_cv_t<std::remove_reference_t<ContainerT>>, ContainerT&>;
    T m_container;

    decltype(detail::size_(m_container)) m_size = detail::size_(m_container);
};
template<typename ContainerT>
Enumerate(ContainerT&&) -> Enumerate<ContainerT>;

template <typename ContainerT>
auto enumerate(ContainerT&& v) {
    using std::begin;
    using std::end;
    using std::size;
    using IterType = std::remove_reference_t<std::remove_cv_t<decltype(begin(v))>>;
    if constexpr (std::is_rvalue_reference_v<decltype(v)>) {
        return detail::enumerate_struct<ContainerT&&, IterType>(std::move(v));
    } else {
        return detail::enumerate_struct<decltype(v), IterType>{begin(v), end(v), size(v)};
    }
}

template <typename ContainerT>
std::vector<value_type<ContainerT>> to_std_vector(ContainerT&& c);

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

template <typename OutT, typename InT, size_t N>
auto static_cast_all(const InT (&container)[N]) {
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
template <typename ContainerT, typename ValueT>
bool contains(const ContainerT& c, const ValueT& value) {
    if constexpr (mleivo::type_traits::has_method_contains_v<ContainerT>) {
        return c.contains(value);
    } else if constexpr (mleivo::type_traits::has_method_count_v<ContainerT>) {
        return c.count(value) != 0;
    } else {
        return std::find(std::begin(c), std::end(c), value) != std::end(c);
    }
}

// filter
template <typename ContainerT, typename Filter>
std::vector<value_type<ContainerT>> filter(ContainerT&& c, Filter&& f) {
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
template <typename ContainerT>
std::vector<value_type<ContainerT>> merge(ContainerT&& c) {
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
template <typename ContainerT, typename T>
auto index_of(const ContainerT& container, T&& predOrItem) {
    using std::cbegin;
    using std::cend;
    if constexpr (std::is_same_v<value_type<ContainerT>, std::decay_t<T>>)
        return std::distance(cbegin(container), std::find(cbegin(container), cend(container), predOrItem));
    else
        return std::distance(cbegin(container),
                             std::find_if(cbegin(container), cend(container), std::forward<T>(predOrItem)));
}

namespace { namespace irange_lazy_impl {
template <typename T, bool IsUnsigned = std::is_unsigned_v<T>>
struct IRangeLazyIter {
    T m_value;
    const T m_step;
    const T m_sign = m_step < T{} ? static_cast<T>(-1) : static_cast<T>(1);

    void operator++() {
        m_value += m_step;
    };
    auto operator*() const noexcept {
        return m_value;
    }

    template <bool X = IsUnsigned>
    bool operator!=(const IRangeLazyIter& rhs) const noexcept {
        if constexpr (X) {
            return *(*this) < *rhs;
        } else {
            return m_sign * *(*this) < m_sign * *rhs;
        }
    }
};

template <typename T>
struct IRangeLazy {
    const T m_a;
    const T m_b;
    const T m_step;

    auto begin() const {
        return IRangeLazyIter<T>{m_a, m_step};
    }
    auto end() const {
        return IRangeLazyIter<T>{m_b, m_step};
    }
};
}} // namespace ::irange_lazy_impl

template <typename I, typename = std::enable_if_t<std::is_integral_v<I>>>
auto irange(const I& a, const I& b, const I& step) {
    return irange_lazy_impl::IRangeLazy<I>{a, b, step};
}

// pop_front
template <typename ContainerT>
void pop_front(ContainerT& v) {
    using std::begin;
    assert(!v.empty());
    v.erase(begin(v));
}

// remove_all
template <typename ContainerT, typename T>
void remove_all(ContainerT& container, T&& predOrItem) {
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

template <typename ContainerT>
void remove_duplicates(ContainerT& container) {
    remove_duplicates(container, std::equal_to{});
}

template <typename ContainerT>
auto reverse(ContainerT&& container) {
    using std::begin;
    using std::end;
    auto out = std::forward<ContainerT>(container);
    std::reverse(begin(out), end(out));
    return out;
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
template <typename ContainerT, typename TransformerT>
auto transform(ContainerT&& c, TransformerT&& t) {
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
template <typename ContainerT>
std::vector<value_type<ContainerT>> to_std_vector(ContainerT&& c) {
    std::vector<value_type<ContainerT>> out;
    if constexpr (std::is_rvalue_reference_v<decltype(c)> && std::is_move_constructible_v<value_type<decltype(c)>>) {
        out.insert(out.end(), std::make_move_iterator(std::begin(c)), std::make_move_iterator(std::end(c)));
    } else {
        std::copy(std::begin(c), std::end(c), std::back_inserter(out));
    }
    return out;
}
} // namespace mleivo::cu

namespace std {
template <typename It>
struct tuple_size<mleivo::cu::detail::iter_value_type<It>> : std::integral_constant<size_t, 2> {};

template <typename It>
struct tuple_element<0, mleivo::cu::detail::iter_value_type<It>> {
    using type = size_t;
};

template <typename It>
struct tuple_element<1, mleivo::cu::detail::iter_value_type<It>> {
    using type = std::remove_reference_t<decltype(*std::declval<It>())>;
};
} // namespace std
