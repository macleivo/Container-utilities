#pragma once

#include <algorithm>
#include <utility>

#include <tuple>
#include <vector>

template <typename...>
class TD;

template <typename ContainerT, typename CallablePipeT,
          typename = decltype(std::declval<CallablePipeT>()(std::declval<ContainerT>()))>
inline decltype(auto) constexpr operator|(ContainerT&& container, CallablePipeT&& f);

namespace mleivo::pipes {

namespace {
template <typename CallT, typename... Args>
struct wrapper {
    std::tuple<Args&&...> m_t;
    wrapper(Args&&... args) : m_t(std::forward<Args>(args)...) {
    }

    template <typename ContainerT>
    inline constexpr decltype(auto) operator()(ContainerT&& container) {
        std::apply(
            [&](auto&&... args) {
                using std::begin;
                using std::end;
                CallT::call(begin(container), end(container), std::forward<decltype(args)>(args)...);
            },
            std::move(m_t));
        return std::forward<decltype(container)>(container);
    }
};
} // namespace

#define MLEIVO_STL_WRAPPER(FUNCTION_NAME)                                                                              \
    namespace {                                                                                                        \
    struct FUNCTION_NAME {                                                                                             \
        template <typename... Args>                                                                                    \
        static inline constexpr decltype(auto) call(Args&&... args) {                                                  \
            std::FUNCTION_NAME(std::forward<decltype(args)>(args)...);                                                 \
        }                                                                                                              \
    };                                                                                                                 \
    }                                                                                                                  \
                                                                                                                       \
    template <typename... Args>                                                                                        \
    inline constexpr decltype(auto) FUNCTION_NAME(Args&&... args) {                                                    \
        return wrapper<struct FUNCTION_NAME, decltype(args)...>{std::forward<decltype(args)>(args)...};                \
    };

MLEIVO_STL_WRAPPER(for_each);
MLEIVO_STL_WRAPPER(reverse);
MLEIVO_STL_WRAPPER(sort);
#undef MLEIVO_STL_WRAPPER

} // namespace mleivo::pipes

template <typename ContainerT, typename CallablePipeT, typename>
inline decltype(auto) constexpr operator|(ContainerT&& container, CallablePipeT&& f) {
    return f(std::forward<ContainerT>(container));
}
