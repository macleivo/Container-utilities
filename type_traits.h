#pragma once

#include <type_traits>

namespace cu::type_traits
{
// value_type_t
template<typename ContainerT>
using value_type = typename std::decay_t<ContainerT>::value_type;

#define GLUE_HELPER(x, y) x##y
#define GLUE(x, y) GLUE_HELPER(x, y)

#define CONTAINER_UTILS_HAS_METHOD(NAME, RETURN_TYPE, ARG)                                                             \
    template<typename ContainerT, typename = void>                                                                     \
    struct GLUE(has_method_, NAME) : std::false_type                                                                   \
    {                                                                                                                  \
    };                                                                                                                 \
                                                                                                                       \
    template<typename ContainerT>                                                                                      \
    struct GLUE(has_method_,                                                                                           \
                NAME)<ContainerT, std::void_t<decltype(std::declval<std::decay_t<ContainerT>>().NAME(ARG))>>           \
        : std::bool_constant<std::is_same_v<bool, decltype(std::declval<std::decay_t<ContainerT>>().NAME(ARG))>>       \
    {                                                                                                                  \
    };                                                                                                                 \
                                                                                                                       \
    template<typename ContainerT>                                                                                      \
    inline constexpr bool GLUE(GLUE(has_method_, NAME), _v) = GLUE(has_method_, NAME)<ContainerT>::value;

CONTAINER_UTILS_HAS_METHOD(contains, bool, std::declval<value_type<ContainerT>>())

CONTAINER_UTILS_HAS_METHOD(count, std::size_t, std::declval<value_type<ContainerT>>())

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
