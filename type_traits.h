#pragma once

#include <type_traits>

namespace cu::type_traits
{
// value_type_t
template<typename ContainerT>
using value_type = typename std::decay_t<ContainerT>::value_type;

// has_method_contains
template<typename ContainerT, typename = void>
struct has_method_contains : std::false_type
{
};

template<typename ContainerT>
struct has_method_contains<ContainerT,
                           std::void_t<decltype(std::declval<std::decay_t<ContainerT>>().contains(
                                   std::declval<value_type<ContainerT>>()))>> : std::true_type
{
};

template<typename ContainerT>
inline constexpr bool has_method_contains_v = has_method_contains<ContainerT>::value;

// has_method_count
template<typename ContainerT, typename = void>
struct has_method_count : std::false_type
{
};

template<typename ContainerT>
struct has_method_count<
        ContainerT,
        std::void_t<decltype(std::declval<std::decay_t<ContainerT>>().count(std::declval<value_type<ContainerT>>()))>>
    : std::true_type
{
};

template<typename ContainerT>
inline constexpr bool has_method_count_v = has_method_count<ContainerT>::value;

// value_types_are_equal
template<typename ContainerT1, typename... ContainerT2ToN>
struct value_types_equal;

template<typename ContainerT>
struct value_types_equal<ContainerT> : std::true_type
{
};

template<typename ContainerT1, typename ContainerT2, typename... ContainerT3ToN>
struct value_types_equal<ContainerT1, ContainerT2, ContainerT3ToN...>
    : std::bool_constant<std::is_same_v<typename std::decay_t<ContainerT1>::value_type,
                                        typename std::decay_t<ContainerT2>::
                                                value_type> && value_types_equal<ContainerT2, ContainerT3ToN...>::value>
{
};

template<typename ContainerT1, typename... ContainerT2ToN>
inline constexpr bool value_types_equal_v = value_types_equal<ContainerT1, ContainerT2ToN...>::value;
}
