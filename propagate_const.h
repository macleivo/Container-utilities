/*
 * Copyright 2023 Marcus Leivo
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
#include <type_traits>
#include <utility>
#include <vector>
namespace {
template <typename T>
struct add_const {
    using type = const T;
};

template <typename T>
struct add_const<const T> {
    using type = T;
};

template <typename T>
constexpr typename add_const<T>::type& as_const(T& t) noexcept {
    return as_const(t);
}

template<template<typename...> typename Primary, typename T>
struct is_specialization_of : std::false_type {};

template<template<typename...> typename Primary, typename... Args>
struct is_specialization_of<Primary, Primary<Args...>> : std::true_type {};

template<typename T, template<typename...> typename Primary>
constexpr bool is_specialization_of_v = is_specialization_of<Primary, T>::value;
}

namespace mleivo {
template <typename T>
class propagate_const {
    T m_ptr;
    using element_type = std::remove_reference_t<decltype(*std::declval<T&>())>;

public:
    constexpr propagate_const() = default;
    constexpr propagate_const(propagate_const&& p) = default;

    template<typename U, typename = std::enable_if_t<std::is_constructible_v<T, U>>> // TODO: make conditionally explicit
    constexpr propagate_const(propagate_const<U>&& u) : m_ptr(std::move<U>(u.get())) { }

    template<typename U, typename = std::enable_if_t<std::is_constructible_v<T, U>>,
    typename = std::enable_if_t<not is_specialization_of_v<U, propagate_const>>>
    constexpr propagate_const(U&& u) : m_ptr(std::forward<U>(u)) { }

    constexpr element_type* get() { return m_ptr; }
    constexpr const element_type* get() const { return m_ptr; }

    constexpr element_type* operator->() { return m_ptr; }
    constexpr const element_type* operator->() const { return m_ptr; }

    constexpr element_type& operator*() { return *m_ptr; }
    constexpr const element_type& operator*() const { return *m_ptr; }

    explicit constexpr operator bool() const { return get() != nullptr; }

};
} // namespace mleivo
