#pragma once

#include <algorithm>
#include <utility>

#include <vector>

template <typename...>
class TD;

template <typename ContainerT, typename CallablePipeT,
          typename = decltype(std::declval<CallablePipeT>()(std::declval<ContainerT>()))>
auto operator|(ContainerT&& container, CallablePipeT&& f);

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

template <typename UnaryFunction>
struct for_each_impl : pipeable {
    template <typename ContainerT>
    constexpr auto operator()(ContainerT&& container) const {
        using std::begin;
        using std::end;
        auto out = std::forward<ContainerT>(container);
        std::for_each(begin(out), end(out), std::forward<UnaryFunction>(m_f));
        return out;
    }

private:
    template <typename UnaryFunction_>
    for_each_impl(UnaryFunction_&& f, std::enable_if_t<std::is_same_v<UnaryFunction, decltype(f)>, int> = 0)
        : m_f(std::forward<decltype(f)>(f)) {
    }

    UnaryFunction m_f;
    template <typename UnaryFunction_>
    friend auto for_each(UnaryFunction_&& f);

    template <typename ContainerT, typename CallablePipeT, typename>
    friend auto operator|(ContainerT&& container, CallablePipeT&& f);
};

template <typename UnaryFunction>
inline auto for_each(UnaryFunction&& f) {
    return [&f](auto&& container) {
        using std::begin;
        using std::end;
        auto out = std::forward<decltype(container)>(container);
        std::for_each(begin(out), end(out), std::forward<decltype(f)>(f));
        return out;
    };
};

} // namespace mleivo::pipes

template <typename ContainerT, typename CallablePipeT, typename>
auto operator|(ContainerT&& container, CallablePipeT&& f) {
    return f(std::forward<ContainerT>(container));
}
