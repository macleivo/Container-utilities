#pragma once

#include <type_traits>

namespace cu::type_traits
{
// value_type_t
template<typename ContainerT>
using value_type = typename std::decay_t<ContainerT>::value_type;

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
}
