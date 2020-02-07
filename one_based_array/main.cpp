
// MIT License
//
// Copyright (c) 2020 degski
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, rhs_, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include <array>
#include <sax/iostream.hpp>
#include <initializer_list>
#include <limits> // For Point2.
#include <type_traits>

#include "one_based_array.hpp"

template<typename T>
struct Point2 {

    using value_type = T;

    value_type x, y;

    Point2 ( ) noexcept : x{ std::numeric_limits<value_type>::quiet_NaN ( ) } { };
    Point2 ( Point2 const & ) noexcept = default;
    Point2 ( Point2 && ) noexcept      = default;
    Point2 ( value_type && x_, value_type && y_ ) noexcept : x{ std::move ( x_ ) }, y{ std::move ( y_ ) } {}

    // template<typename SfmlVec>
    // Point2 ( SfmlVec && v_ ) noexcept : x{ std::move ( v_.x ) }, y{ std::move ( v_.y ) } {}

    [[maybe_unused]] Point2 & operator= ( Point2 const & ) noexcept = default;
    [[maybe_unused]] Point2 & operator= ( Point2 && ) noexcept = default;

    [[nodiscard]] bool operator== ( Point2 const & p_ ) const noexcept { return x == p_.x and y == p_.y; }
    [[nodiscard]] bool operator!= ( Point2 const & p_ ) const noexcept { return x != p_.x or y != p_.y; }

    [[maybe_unused]] Point2 & operator+= ( Point2 const & p_ ) noexcept {
        x += p_.x;
        y += p_.y;
        return *this;
    }
    [[maybe_unused]] Point2 & operator-= ( Point2 const & p_ ) noexcept {
        x -= p_.x;
        y -= p_.y;
        return *this;
    }

    template<typename Stream>
    [[maybe_unused]] friend Stream & operator<< ( Stream & out_, Point2 const & p_ ) noexcept {
        if ( not std::isnan ( p_.x ) )
            out_ << '<' << p_.x << ' ' << p_.y << '>';
        else
            out_ << "<* *>";
        return out_;
    }
};

namespace detail {
template<typename ValueType, typename RandomIt, typename Compare>
[[nodiscard]] RandomIt next ( RandomIt b_, std::intptr_t const index_ ) noexcept {
    b_ += 2 * index_;
    return b_ + not Compare ( ) ( b_[ 0 ], b_[ 1 ] );
}
} // namespace detail
template<typename ValueType, typename RandomIt, typename Compare = std::less<ValueType>>
[[nodiscard]] RandomIt it_find ( RandomIt const b_, RandomIt const e_, ValueType const & v_ ) noexcept {
    for ( RandomIt b = b_; b <= e_; b = detail::next<ValueType, RandomIt, Compare> ( b, b - b_ ) )
        if ( b[ 0 ] == v_ )
            return b;
    return e_;
}

//                    +---+
//                    | A |
//                    +---+
//                   /     \
//              +---+       +---+
//              | B |       | E |
//              +---+       +---+
//             /     \      ^
//        +---+       +---+ |
//        | C |       | D | |
//        +---+       +---+ |
//            |       ^   | |
//            +-------+   +-+

struct click final {
    int i = 0;

    explicit click ( ) noexcept { std::cout << "c'tor" << nl; }
    ~click ( ) noexcept { std::cout << "d'tor" << nl; }

    click ( click const & other_ ) noexcept : i ( other_.i ) { std::cout << "copy c'tor" << nl; }
    click ( click && other_ ) noexcept : i ( std::forward<int> ( other_.i ) ) { std::cout << "move c'tor" << nl; }

    click & operator= ( click const & other_ ) noexcept {
        i = other_.i;
        std::cout << "copy asign" << nl;
        return *this;
    }

    click & operator= ( click && other_ ) noexcept {
        i = std::move ( other_.i );
        std::cout << "move asign" << nl;
        return *this;
    }
};

int main ( ) {

    constexpr sax::one_based_array<int, 45> a{ }; // 35 * 4 = 140

    a[ 5 ] = 159;

    // std::cout << a[ 5 ] << nl;

    sax::one_based_array<int, 45> b = std::move ( a );

    std::cout << b[ 5 ] << ' ' << ( a == b ) << nl;

    return EXIT_SUCCESS;
}
