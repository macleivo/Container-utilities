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

#include <array>
#include <iterator>
#include <utility>

struct Bindable {
  Bindable(int i, int j)
    : a(i)
    , b(j){};
  int a;
  int b;
};

struct Cont {
    struct asd {
        size_t m_i;
        int* m_ptr;
    };
    struct Iterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = int;
        using pointer = value_type*;
        using reference = value_type&;
        Iterator(size_t i, pointer ptr) : m_data({i, ptr}) {}

        asd& operator*() { return m_data; }
        Iterator& operator++(){ ++m_data.m_i; ++m_data.m_ptr; return *this; }
        Iterator operator++(int){ auto tmp = *this; ++(*this); return tmp; }
        friend bool operator==(const Iterator& lhs, const Iterator& rhs) { return lhs.m_data.m_ptr == rhs.m_data.m_ptr;}
        friend bool operator!=(const Iterator& lhs, const Iterator& rhs) { return !(lhs.m_data.m_ptr == rhs.m_data.m_ptr);}
        private:
            asd m_data;
    };

    auto begin() { return Iterator(0, &m_array[0]);}
    auto end()
    {
      using std::size;
      return Iterator(10, &m_array[size(m_array)]);
    }

  private:
    int m_array[5] = {1, 1, 1, 1, 1};
};

template<size_t index>
std::tuple_element_t<index, Cont::asd>& get(Cont::asd& asd)
{
    if constexpr(index == 0) return asd.m_i;
    if constexpr(index == 1) return *asd.m_ptr;
}

namespace std
{
template<>
struct tuple_size<::Cont::asd> { static inline constexpr size_t value = 2; };

template<> struct tuple_element<0, ::Cont::asd> { using type = size_t; };
template<> struct tuple_element<1, ::Cont::asd> { using type = int&; };
}
