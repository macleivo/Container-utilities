#pragma once

#include <type_traits>

namespace cu::type_traits
{
// value_type_t
template<typename ContainerT>
using value_type = typename std::decay_t<ContainerT>::value_type;

#define GLUE_HELPER(x, y) x##y
#define GLUE(x, y) GLUE_HELPER(x, y)
#define RETURN_TYPE(METHOD_NAME, ARG) decltype(std::declval<std::decay_t<ContainerT>>().METHOD_NAME(ARG))

#define CONTAINER_UTILS_HAS_METHOD(METHOD_NAME, WANTED_RETURN_TYPE, ARG) \
    template<typename ContainerT, typename = void> \
    struct GLUE(has_method_, METHOD_NAME) : std::false_type \
    { \
    }; \
\
    template<typename ContainerT> \
    struct GLUE(has_method_, METHOD_NAME)<ContainerT, std::void_t<RETURN_TYPE(METHOD_NAME, ARG)>> \
        : std::bool_constant<std::is_same_v<WANTED_RETURN_TYPE, RETURN_TYPE(METHOD_NAME, ARG)>> \
    { \
    }; \
\
    template<typename ContainerT> \
    inline constexpr bool GLUE(GLUE(has_method_, METHOD_NAME), _v) = GLUE(has_method_, METHOD_NAME)<ContainerT>::value;

CONTAINER_UTILS_HAS_METHOD(contains, bool, std::declval<value_type<ContainerT>>())
CONTAINER_UTILS_HAS_METHOD(count, size_t, std::declval<value_type<ContainerT>>())

#undef GLUE_HELPER
#undef GLUE
#undef RETURN_TYPE
#undef CONTAINER_UTILS_HAS_METHOD

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
