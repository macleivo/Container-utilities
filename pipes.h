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
struct pipeable {
    using is_pipeable = std::true_type;
};

struct reverse : pipeable {
    template <typename ContainerT>
    constexpr auto operator()(ContainerT&& container) const {
        using std::begin;
        using std::end;
        auto out = std::forward<ContainerT>(container);
        std::reverse(begin(out), end(out));
        return out;
    }
};

struct for_each {
    template <typename... Args>
    static inline constexpr decltype(auto) call(Args&&... args) {
        std::for_each(std::forward<decltype(args)>(args)...);
    }
};

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
            m_t);
        return std::forward<decltype(container)>(container);
    }
};

template <typename UnaryFunction>
inline constexpr decltype(auto) for_each(UnaryFunction&& f) {
    return wrapper<struct for_each, decltype(f)>{std::forward<decltype(f)>(f)};
};

} // namespace mleivo::pipes

template <typename ContainerT, typename CallablePipeT, typename>
inline decltype(auto) constexpr operator|(ContainerT&& container, CallablePipeT&& f) {
    return f(std::forward<ContainerT>(container));
}
