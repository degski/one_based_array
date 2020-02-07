
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
#include <tuple>
#include <type_traits>

#include "one_based_array.hpp"

#define ever                                                                                                                       \
    ;                                                                                                                              \
    ;

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

template<typename ValueType, std::size_t Base = 1>
struct beap {

    private:
    using data_type = std::vector<ValueType>;

    // Current height of beap. Note that height is defined as
    // distance between consecutive layers, so for single - element
    // beap height is 0, and for empty, we initialize it to
    // std::numeric_limits<size_type>::max ( ).

    public:
    using value_type             = typename data_type::value_type;
    using size_type              = int;
    using difference_type        = int;
    using reference              = typename data_type::reference;
    using const_reference        = typename data_type::const_reference;
    using pointer                = typename data_type::pointer;
    using const_pointer          = typename data_type::const_pointer;
    using iterator               = typename data_type::iterator;
    using const_iterator         = typename data_type::const_iterator;
    using reverse_iterator       = typename data_type::reverse_iterator;
    using const_reverse_iterator = typename data_type::const_reverse_iterator;

    using python_span_type = std::tuple<value_type, value_type>;

    // The i'th block consists of the i elements stored from position
    // ( i( i - 1 ) / 2 + 1 ) through position i( i + 1 ) / 2. "
    // These formulas use 1 - based i, and return 1 - based array index.
    template<size_type B = Base>
    constexpr python_span_type python_span_1_based ( size_type i_ ) const noexcept {
        auto i = i_ * i_ - i_ - B;
        return { i, i + 2 * i_ };
    }

    // Convert to use sane zero_v-based indexes both for "block" (span)
    // and array.
    template<size_type B = Base>
    constexpr python_span_type python_span ( size_type i_ ) const noexcept {
        ++i_;
        auto i = i_ * i_ - i_ - B;
        return { i - 1, i + 2 * i_ - 1 };
    }

    static constexpr size_type zero_v = { 0 }, one_v = { 1 };

    [[nodiscard]] size_type size ( ) const noexcept { return static_cast<int> ( arr.size ( ) ); }

    // Search for element x in beap. If not found, return None.
    // Otherwise, return tuple of (idx, height) with array index
    // and span height at which the element was found. (Span height
    // is returned because it may be needed for some further
    // operations, to avoid square root operation which is otherwise
    // needed to convert array index to it.)
    [[nodiscard]] python_span_type search ( value_type const & x ) const noexcept {
        size_type h         = height;
        auto [ start, end ] = span ( h );
        size_type idx       = start;
        for ( ever ) {
            iters += 1;
            if ( x > arr[ idx ] ) {
                // If x is less than the element under consideration, move left
                // one_v position along the row.
                // These rules are given for weirdly mirrored matrix. They're also
                // for min beap, we so far implement max beap.
                // So: if x is greater than, and move up along the column.
                if ( idx == end )
                    return { zero_v, zero_v };
                size_type diff = idx - start;
                h -= 1;
                auto [ start, end ] = python_span ( h );
                idx                 = start + diff;
                continue;
            }
            else if ( x < arr[ idx ] ) {
                // If x exceeds the element, either move down one_v position along the column or if
                // this is not possible (because we are on the diagonal) then move left and down one_v position
                // each.
                // => less, move right along the row, or up and right
                if ( idx == size ( ) - 1 ) {
                    size_type diff = idx - start;
                    h -= 1;
                    auto [ start, end ] = python_span ( h );
                    idx                 = start + diff;
                    continue;
                }
                size_type diff              = idx - start;
                auto [ new_start, new_end ] = python_span ( h + 1 );
                size_type new_idx           = new_start + diff + 1;
                if ( new_idx < size ( ) ) {
                    h += 1;
                    start = new_start;
                    end   = new_end;
                    idx   = new_idx;
                    continue;
                }
                if ( idx == end )
                    return { zero_v, zero_v };
                idx += 1;
                continue;
            }
            else {
                return { idx, h };
            }
        }
    }

    // Percolate an element up the beap.
    [[nodiscard]] size_type filter_up ( size_type index_, size_type h_ ) noexcept {
        value_type v = arr[ index_ ];
        while ( h_ ) {
            iters += 1;
            auto [ start, end ] = python_span ( h_ );
            size_type left_p = zero_v, right_p = zero_v, val_l = zero_v, val_r = zero_v;
            size_type diff       = index_ - start;
            auto [ st_p, end_p ] = python_span ( h_ - one_v );
            if ( index_ != start ) {
                left_p = st_p + diff - 1;
                val_l  = arr[ left_p ];
            }
            if ( index_ != end ) {
                right_p = st_p + diff;
                val_r   = arr[ right_p ];
            }
            if ( val_l != zero_v and v > val_l and ( val_r == zero_v or val_l < val_r ) ) {
                std::swap ( arr[ v ], arr[ left_p ] );
                index_ = left_p;
                h_ -= 1;
            }
            else if ( val_r != zero_v and v > val_r ) {
                std::swap ( arr[ v ], arr[ right_p ] );
                index_ = right_p;
                h_ -= 1;
            }
            else {
                return index_;
            }
        }
        assert ( index_ == zero_v );
        return index_;
    }

    // Percolate an element down the beap.
    [[nodiscard]] size_type filter_down ( size_type index_, size_type h_ ) noexcept {
        while ( h_ < height - one_v ) {
            iters += 1;
            auto [ start, end ]  = python_span ( h_ );
            size_type diff       = index_ - start;
            auto [ st_c, end_c ] = python_span ( h_ + one_v );
            size_type left_c = st_c + diff, right_c = zero_v, val_l = zero_v, val_r = zero_v;
            if ( left_c < size ( ) ) {
                val_l   = arr[ left_c ];
                right_c = left_c + one_v;
                if ( right_c >= size ( ) )
                    right_c = zero_v;
                else
                    val_r = arr[ right_c ];
            }
            else {
                left_c = zero_v;
            }
            size_type v = arr[ index_ ];
            if ( val_l != zero_v and v < val_l and ( val_r == zero_v or val_l > val_r ) ) {
                std::swap ( arr[ v ], arr[ left_c ] );
                index_ = left_c;
                h_ += 1;
            }
            else if ( val_r != zero_v and v < val_r ) {
                std::swap ( arr[ v ], arr[ right_c ] );
                index_ = right_c;
                h_ += 1;
            }
            else {
                return index_;
            }
            return index_;
        }
    }

    // If last array element as at the span end, then adding
    // new element grows beap height.
    [[nodiscard]] size_type insert ( value_type const & v_ ) {
        auto [ start, end ] = python_span ( height );
        size_type index     = size ( ) - one_v;
        height += index == end;
        arr.push_back ( v_ );
        return filter_up ( index, height );
    }

    // Output.

    template<typename Stream>
    [[maybe_unused]] friend Stream & operator<< ( Stream & out_, beap const & beap_ ) noexcept {
        std::for_each ( beap_.arr.cbegin ( ), beap_.arr.cend ( ), [ &out_ ] ( auto & e ) { out_ << e << sp; } );
        return out_;
    }

    data_type arr;
    size_type height = std::numeric_limits<size_type>::max ( ), iters = zero_v;
};

int main ( ) {

    beap<int> a;

    int c = 0;
    for ( auto & e : a.arr )
        e = ++c;

    std::cout << a << nl;

    return EXIT_SUCCESS;
}
