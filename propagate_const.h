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
namespace {
template <typename T>
struct S {
    using type = const T;
};

template <typename T>
struct S<const T> {
    using type = T;
};

template <typename T>
constexpr typename S<T>::type& as_const(T& t) noexcept {
    return as_const(t);
}
} // namespace

namespace mleivo {
template <typename T>
class pc {
    T m_ptr;

public:
    pc(T ptr) : m_ptr(ptr) {
    }
    decltype(auto) operator->() {
        return m_ptr;
    }
    decltype(auto) operator->() const {
        return as_const(m_ptr);
    }
    decltype(auto) operator*() {
        return *m_ptr;
    }
    decltype(auto) operator*() const {
        return as_const(*m_ptr);
    }

    decltype(auto) get() {
        return m_ptr;
    }
};
} // namespace mleivo
