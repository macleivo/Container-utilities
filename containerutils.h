#pragma once

#include <type_traits>

namespace cu
{
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

template<typename ContainerT>
std::vector<typename std::decay_t<ContainerT>::value_type> merge(ContainerT&& c)
{
    std::vector<typename std::decay_t<ContainerT>::value_type> out;
    if constexpr (std::is_rvalue_reference_v<ContainerT>)
    {
        out.insert(out.end(), std::make_move_iterator(std::begin(c)), std::make_move_iterator(std::end(c)));
    }
    else
    {
        out.insert(out.end(), std::begin(c), std::end(c));
    }
    return out;
}

template<typename ContainerT1, typename... ContainerT2ToN>
std::enable_if_t<value_types_equal<ContainerT1, ContainerT2ToN...>::value,
                 std::vector<typename std::decay_t<ContainerT1>::value_type>>
merge(ContainerT1&& c1, ContainerT2ToN&&... c2ToN)
{
    auto out = merge(std::forward<ContainerT1>(c1));
    auto mergedTail = merge(std::forward<ContainerT2ToN>(c2ToN)...);
    out.insert(out.end(), std::make_move_iterator(std::begin(mergedTail)),
               std::make_move_iterator(std::end(mergedTail)));

    return out;
}
}
