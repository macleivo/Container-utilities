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

#include <type_traits>

namespace mleivo::type_traits
{
template<typename...>
inline constexpr bool always_false_v = false;

template<typename Key1, typename Value1, typename... KeyValues>
struct type_map
{
  template<typename Key>
  using value =
    std::conditional_t<std::is_same_v<Key, Key1>,
                       Value1,
                       typename type_map<KeyValues...>::template value<Key>>;
};

template<typename Key1, typename Value1>
struct type_map<Key1, Value1>
{
  template<typename Key>
  using value = std::conditional_t<std::is_same_v<Key, Key1>,
                                   Value1,
                                   std::enable_if<always_false_v<Key>>>;
};

// value_type_t
template<typename ContainerT>
using value_type = typename std::decay_t<ContainerT>::value_type;

#define GLUE_HELPER(x, y) x##y
#define GLUE(x, y) GLUE_HELPER(x, y)
#define RETURN_TYPE(METHOD_NAME, ARGS...) decltype(std::declval<std::decay_t<ContainerT>>().METHOD_NAME(ARGS))

#define CONTAINER_UTILS_HAS_METHOD(METHOD_NAME, WANTED_RETURN_TYPE, ARGS...) \
    template<typename ContainerT, typename = void> \
    struct GLUE(has_method_, METHOD_NAME) : std::false_type \
    { \
    }; \
\
    template<typename ContainerT> \
    struct GLUE(has_method_, METHOD_NAME)<ContainerT, std::void_t<RETURN_TYPE(METHOD_NAME, ARGS)>> \
        : std::bool_constant<std::is_same_v<WANTED_RETURN_TYPE, RETURN_TYPE(METHOD_NAME, ARGS)>> \
    { \
    }; \
\
    template<typename ContainerT> \
    inline constexpr bool GLUE(GLUE(has_method_, METHOD_NAME), _v) = GLUE(has_method_, METHOD_NAME)<ContainerT>::value;

CONTAINER_UTILS_HAS_METHOD(contains, bool, std::declval<value_type<ContainerT>>())
CONTAINER_UTILS_HAS_METHOD(count, size_t, std::declval<value_type<ContainerT>>())
CONTAINER_UTILS_HAS_METHOD(push_back, void, std::declval<value_type<ContainerT>>())

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
