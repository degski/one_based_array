
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
#include <span>
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

template<typename ValueType>
struct beap {

    private:
    using data_type = std::vector<ValueType>;

    // Current height of beap. Note that height is defined as
    // distance between consecutive layers, so for single - element
    // beap height is 0, and for empty, we initialize it to
    // std::numeric_limits<size_type>::max ( ).

    public:
    using value_type             = typename data_type::value_type;
    using size_type              = typename data_type::size_type;
    using difference_type        = typename data_type::difference_type;
    using reference              = typename data_type::reference;
    using const_reference        = typename data_type::const_reference;
    using pointer                = typename data_type::pointer;
    using const_pointer          = typename data_type::const_pointer;
    using iterator               = typename data_type::iterator;
    using const_iterator         = typename data_type::const_iterator;
    using reverse_iterator       = typename data_type::reverse_iterator;
    using const_reverse_iterator = typename data_type::const_reverse_iterator;

    // The i'th block consists of the i elements stored from position
    // ( i( i - 1 ) / 2 + 1 ) through position i( i + 1 ) / 2. "
    // These formulas use 1 - based i, and return 1 - based array index.
    constexpr std::span<value_type> span_1_based ( size_type i_ ) const noexcept { return { arr.data ( ) + i * i - i, i << 1 }; }

    // Convert to use sane zero-based indexes both for "block" (span)
    // and array.
    constexpr std::span<value_type> span ( size_type i_ ) const noexcept {
        auto ii = i_ + 1;
        return { arr.data ( ) + i_ * i_ + ii, ii << 1 };
    }

    auto filter_up ( size_type idx_, size_type h_ ) {

        auto v = arr[ idx_ ];

        while ( h ) {

            iters += 1;

            auto s         = span ( h );
            pointer left_p = nullptr, right_p = nullptr;
            value_type val_l = val_r = { };

            auto diff  = idx - start st_p;
            auto end_p = span ( h - 1 );

            if idx
                != start : left_p = st_p + diff - 1 val_l = self.arr[ left_p ] if idx != end
                    : right_p = st_p + diff val_r = self.arr[ right_p ]

                                                    log.debug ( "filter_up: left_p: %s (val: %s) right_p: %s (val: %s)", left_p,
                                                                val_l, right_p, val_r )

                                                        if val_l is not None and
                                                    v > val_l and
                                                    ( val_r is None or val_l < val_r ) :
                    self.arr[ left_p ],
                           self.arr[ idx ]        = self.arr[ idx ],
                           self.arr[ left_p ] idx = left_p h -= 1 elif val_r is not None and v > val_r : self.arr[ right_p ],
                           self.arr[ idx ] = self.arr[ idx ], self.arr[ right_p ] idx = right_p h -= 1 else : return
        }
    }

    // If last array element as at the span end, then adding
    // new element grows beap height.
    auto insert ( value_type v_ ) {
        height += std::addressof ( arr.back ( ) ) == std::addressof ( span ( height ).back ( ) );
        arr.push_back ( v_ );
        return filter_up ( arr.size ( ) - 1ull, height );
    }

    data_type arr;
    size_type height = std::numeric_limits<size_type>::max ( ), iters = 0ull;
};

int main ( ) {

    beap<int, 32> a;

    int c = 0;
    for ( auto & e : a.arr )
        e = ++c;

    std::cout << a.arr << nl;

    return EXIT_SUCCESS;
}
