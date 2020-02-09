
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
#include <optional>
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

    value_type v_, y;

    Point2 ( ) noexcept : v_{ std::numeric_limits<value_type>::quiet_NaN ( ) } { };
    Point2 ( Point2 const & ) noexcept = default;
    Point2 ( Point2 && ) noexcept      = default;
    Point2 ( value_type && v_, value_type && y_ ) noexcept : v_{ std::move ( v_ ) }, y{ std::move ( y_ ) } {}

    // template<typename SfmlVec>
    // Point2 ( SfmlVec && v_ ) noexcept : v_{ std::move ( v_.v_ ) }, y{ std::move ( v_.y ) } {}

    [[maybe_unused]] Point2 & operator= ( Point2 const & ) noexcept = default;
    [[maybe_unused]] Point2 & operator= ( Point2 && ) noexcept = default;

    [[nodiscard]] bool operator== ( Point2 const & p_ ) const noexcept { return v_ == p_.v_ and y == p_.y; }
    [[nodiscard]] bool operator!= ( Point2 const & p_ ) const noexcept { return v_ != p_.v_ or y != p_.y; }

    [[maybe_unused]] Point2 & operator+= ( Point2 const & p_ ) noexcept {
        v_ += p_.v_;
        y += p_.y;
        return *this;
    }
    [[maybe_unused]] Point2 & operator-= ( Point2 const & p_ ) noexcept {
        v_ -= p_.v_;
        y -= p_.y;
        return *this;
    }

    template<typename Stream>
    [[maybe_unused]] friend Stream & operator<< ( Stream & out_, Point2 const & p_ ) noexcept {
        if ( not std::isnan ( p_.v_ ) )
            out_ << '<' << p_.v_ << ' ' << p_.y << '>';
        else
            out_ << "<* *>";
        return out_;
    }
};

namespace detail {
template<typename ValueType, typename RandomIt, typename Compare>
[[nodiscard]] RandomIt next ( RandomIt b_, std::intptr_t const idx_ ) noexcept {
    b_ += 2 * idx_;
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

template<typename ValueType, typename Compare = std::less<ValueType>>
struct beap {

    private:
    using data_type = std::vector<ValueType>;

    // Current height of beap. Note that height is defined as
    // distance between consecutive layers, so for single - element
    // beap height is 0, and for empty, we initialize it to - 1.

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

    struct span_type {
        size_type start, end;
    };

    using compare = Compare;

    beap ( ) noexcept        = default;
    beap ( beap const & b_ ) = default;
    beap ( beap && b_ )      = default;

    template<typename ForwardIt>
    beap ( ForwardIt b_, ForwardIt e_ ) : arr ( b_, e_ ) {}

    [[maybe_unused]] beap & operator= ( beap const & b_ ) = default;
    [[maybe_unused]] beap & operator= ( beap && b_ ) = default;

    // The i'th block consists of the i elements stored from position
    // ( i * ( i - 1 ) / 2 + 1 ) through position i * ( i + 1 ) / 2.
    // These formulas use 1 - based i, and return 1 - based array index.
    [[nodiscard]] constexpr span_type span_1_based ( size_type i_ ) const noexcept {
        return { i_ * ( i_ - one_v ) / two_v + one_v, i_ * ( i_ + one_v ) / two_v };
    }

    [[nodiscard]] span_type next_span ( span_type span_ ) const noexcept {
        auto tmp = span_.end + one_v;
        return { tmp, two_v * tmp - span_.start };
    }
    [[nodiscard]] span_type prev_span ( span_type span_ ) const noexcept {
        return { two_v * span_.start - span_.end, span_.start - one_v };
    }

    [[nodiscard]] size_type next_span_start ( span_type const & span_ ) const noexcept { return span_.end + one_v; }
    [[nodiscard]] size_type prev_span_start ( span_type const & span_ ) const noexcept { return two_v * span_.start - span_.end; }

    // Convert to use sane zero_v-based indexes both for "block" (span)
    // and array.
    [[nodiscard]] constexpr span_type span ( size_type i_ ) const noexcept {
        i_ += one_v;
        return { i_ * ( i_ - one_v ) / two_v, i_ * ( i_ + one_v ) / two_v - one_v };
        // auto [ start, end ] = span_1_based ( i_ + one_v );
        // return { start - one_v, end - one_v };
    }

    static constexpr size_type minus_one_v = { -1 }, zero_v = { 0 }, one_v = { 1 }, two_v = { 2 };

    // Search for element v_ in beap. If not found, return zero-span.
    // Otherwise, return tuple of (idx, height) with array index
    // and span height at which the element was found. (Span height
    // is returned because it may be needed for some further
    // operations, to avoid square root operation which is otherwise
    // needed to convert array index to it.)
    [[nodiscard]] span_type search ( value_type const & v_ ) const noexcept {
        auto compare_3way = [] ( const_reference p0_, const_reference p1_ ) noexcept {
            return Compare ( ) ( p0_, p1_ ) ? -1 : Compare ( ) ( p1_, p0_ ) ? +1 : 0;
        };
        size_type h         = height;
        span_type curr_span = span ( h );
        size_type idx       = curr_span.start;
        for ( ever ) {
            const_reference at_idx = at ( idx );
            if ( bool unequal = v_ != at_idx; unequal ) {
                if ( bool greater = not compare ( ) ( v_, at_idx ); greater ) {
                    // If v_ is less than the element under consideration, move left
                    // one_v position along the row.
                    // These rules are given for weirdly mirrored matrix. They're also
                    // for min beap, we so far implement max beap.
                    // So: if v_ is greater than, and move up along the column.
                    if ( idx == curr_span.end )
                        return { zero_v, zero_v };
                    size_type diff = idx - curr_span.start;
                    h -= one_v;
                    idx = prev_span_start ( curr_span ) + diff;
                    continue;
                }
                else {
                    // If v_ exceeds the element, either move down one_v position along the column or if
                    // this is not possible (because we are on the diagonal) then move left and down one_v position
                    // each.
                    // => less, move right along the row, or up and right
                    if ( idx == size ( ) - one_v ) {
                        size_type diff = idx - curr_span.start;
                        h -= one_v;
                        idx = prev_span_start ( curr_span ) + diff;
                        continue;
                    }
                    size_type diff     = idx - curr_span.start;
                    span_type new_span = next_span ( curr_span );
                    size_type new_idx  = new_span.start + diff + one_v;
                    if ( new_idx < size ( ) ) {
                        h += one_v;
                        curr_span = new_span;
                        idx       = new_idx;
                        continue;
                    }
                    if ( idx == curr_span.end )
                        return { zero_v, zero_v };
                    idx += one_v;
                    continue;
                }
            }
            else {
                std::cout << "found " << at ( idx ) << nl;
                return { idx, h };
            }
        }
    }

    [[nodiscard]] span_type search_2 ( value_type const & v_ ) const noexcept {
        size_type h         = height;
        span_type curr_span = span ( h );
        size_type idx       = curr_span.start;
        for ( ever ) {
            if ( v_ > arr[ idx ] ) {
                if ( idx == curr_span.end )
                    return { zero_v, zero_v };
                size_type diff = idx - curr_span.start;
                h -= one_v;
                curr_span = span ( h );
                idx       = curr_span.start + diff;
                continue;
            }
            else if ( v_ < arr[ idx ] ) {
                if ( idx == size ( ) - one_v ) {
                    size_type diff = idx - curr_span.start;
                    h -= one_v;
                    curr_span = span ( h );
                    idx       = curr_span.start + diff;
                    continue;
                }
                size_type diff     = idx - curr_span.start;
                span_type new_span = next_span ( curr_span );
                size_type new_idx  = new_span.start + diff + one_v;
                if ( new_idx < size ( ) ) {
                    h += one_v;
                    curr_span = new_span;
                    idx       = new_idx;
                    continue;
                }
                if ( idx == curr_span.end )
                    return { zero_v, zero_v };
                idx += one_v;
                continue;
            }
            else {
                std::cout << "found " << at ( idx ) << nl;
                return { idx, h };
            }
        }
    }

    // Percolate an element up the beap.
    [[nodiscard]] size_type filter_up ( size_type idx_, size_type h_ ) noexcept {
        pointer v = arr.data ( ) + idx_;
        while ( h_ ) {
            auto [ start, end ] = span ( h_ );
            size_type diff = idx_ - start, left_p = zero_v, right_p = zero_v, val_l = zero_v, val_r = zero_v;
            auto [ st_p, end_p ] = span ( h_ - one_v );
            if ( idx_ != start ) {
                left_p = st_p + diff - one_v;
                val_l  = left_p;
            }
            if ( idx_ != end ) {
                right_p = st_p + diff;
                val_r   = right_p;
            }
            if ( val_l != zero_v and at ( v ) > at ( val_l ) and ( val_r == zero_v or at ( val_l ) < at ( val_r ) ) ) {
                std::swap ( at ( v ), at ( left_p ) );
                idx_ = left_p;
                h_ -= one_v;
            }
            else if ( val_r != zero_v and at ( v ) > at ( val_r ) ) {
                std::swap ( at ( v ), at ( right_p ) );
                idx_ = right_p;
                h_ -= one_v;
            }
            else {
                return idx_;
            }
        }
        assert ( idx_ == zero_v );
        return idx_;
    }

    // Percolate an element down the beap.
    [[nodiscard]] size_type filter_down ( size_type idx_, size_type h_ ) noexcept {
        while ( h_ < height - one_v ) {
            auto [ start, end ]  = span ( h_ );
            auto [ st_c, end_c ] = span ( h_ + one_v );
            size_type diff = idx_ - start, left_c = st_c + diff, right_c = zero_v, val_l = zero_v, val_r = zero_v;
            if ( left_c < size ( ) ) {
                val_l   = left_c;
                right_c = left_c + one_v;
                if ( right_c >= size ( ) )
                    right_c = zero_v;
                else
                    val_r = right_c;
            }
            else {
                left_c = zero_v;
            }
            pointer v = arr.data ( ) + idx_;
            if ( val_l != zero_v and at ( v ) < at ( val_l ) and ( val_r == zero_v or at ( val_l ) > at ( val_r ) ) ) {
                std::swap ( at ( v ), at ( left_c ) );
                idx_ = left_c;
                h_ += one_v;
            }
            else if ( val_r != zero_v and at ( v ) < at ( val_r ) ) {
                std::swap ( at ( v ), at ( right_c ) );
                idx_ = right_c;
                h_ += one_v;
            }
            else {
                return idx_;
            }
            return idx_;
        }
    }

    // If last array element as at the span end, then adding
    // new element grows beap height.
    [[nodiscard]] size_type insert ( value_type const & v_ ) {
        auto [ start, end ] = span ( height );
        std::cout << "inserting " << v_ << " start " << start << " end " << end << nl;
        // If last array element as at the span end, then adding
        // new element grows beap height.
        size_type eos = size ( ) - one_v;
        height += static_cast<int> ( eos == end );
        arr.push_back ( v_ );
        std::cout << "filter up " << v_ << " idx " << ( eos + one_v ) << " height " << height << nl;
        return filter_up ( eos + one_v, height );
    }

    // Remove element with array index idx at the beap span of height h.
    // The height needs to be passed to avoid square root operation to find it.
    std::optional<value_type> remove ( size_type idx_, size_type h_ ) noexcept {
        auto [ start, end ] = span ( height );
        // If last array element as at the span start, then removing
        // it decreases the beap height.
        height -= ( end_of_storage ( ) == start );
        value_type && last = pop ( );
        if ( idx_ == size ( ) )
            return { };
        value_type && removed = std::move ( at ( idx_ ) );
        at ( idx_ )           = std::move ( last );
        if ( size_type out_idx = filter_down ( idx_, h_ ); out_idx == idx_ )
            filter_up ( idx_, h_ );
        return { std::move ( removed ) };
    }
    // Remove element with value of v from beap.
    std::optional<value_type> remove ( value_type const & v_ ) noexcept {
        auto [ idx, h ] = search ( v_ );
        if ( not idx or not h )
            return { };
        return remove ( idx, h );
    }

    [[nodiscard]] size_type size ( ) const noexcept { return static_cast<int> ( arr.size ( ) ); }
    [[nodiscard]] size_type end_of_storage ( ) const noexcept { return static_cast<int> ( arr.size ( ) ) - one_v; }

    // Iterators.

    public:
    [[nodiscard]] iterator begin ( ) noexcept { return arr.begin ( ); }
    [[nodiscard]] const_iterator cbegin ( ) const noexcept { return arr.begin ( ); }

    [[nodiscard]] iterator end ( ) noexcept { return arr.end ( ); }
    [[nodiscard]] const_iterator cend ( ) const noexcept { return arr.end ( ); }

    [[nodiscard]] iterator rbegin ( ) noexcept { return arr.rbegin ( ); }
    [[nodiscard]] const_iterator crbegin ( ) const noexcept { return arr.rbegin ( ); }

    [[nodiscard]] iterator rend ( ) noexcept { return arr.rend ( ); }
    [[nodiscard]] const_iterator crend ( ) const noexcept { return arr.rend ( ); }

    // Access.

    [[nodiscard]] reference front ( ) noexcept { return arr.front ( ); }
    [[nodiscard]] const_reference front ( ) const noexcept { return arr.front ( ); }

    [[nodiscard]] reference back ( ) noexcept { return arr.back ( ); }
    [[nodiscard]] const_reference back ( ) const noexcept { return arr.back ( ); }

    // Output.

    template<typename Stream>
    [[maybe_unused]] friend Stream & operator<< ( Stream & out_, beap const & beap_ ) noexcept {
        std::for_each ( beap_.cbegin ( ), beap_.cend ( ), [ &out_ ] ( auto & e ) { out_ << e << sp; } );
        return out_;
    }

    private:
    [[nodiscard]] const_reference at ( pointer p_ ) const noexcept { return p_[ 0 ]; }
    [[nodiscard]] reference at ( pointer p_ ) noexcept { return p_[ 0 ]; }

    [[nodiscard]] const_reference at ( size_type s_ ) const noexcept { return arr.data ( )[ s_ ]; }
    [[nodiscard]] reference at ( size_type s_ ) noexcept { return arr.data ( )[ s_ ]; }

    value_type && pop ( ) {
        value_type && last = std::move ( arr.back ( ) );
        arr.pop_back ( );
        return std::move ( last );
    }

    data_type arr;
    size_type height = minus_one_v;
};

template<typename T>
void pr ( T const & b_, int i_ ) {
    auto s = b_.search ( i_ );
    std::cout << "i " << i_ << " " << s.start << " " << s.end << nl;
}

// Data from Ian Munro's "ImpSODA06.ppt" presentation, Slide 3,
// with mistake corrected( 21 and 22 not in beap order ).
std::array<int, 24> data = { 72, 68, 63, 44, 62, 55, 33, 22, 32, 51, 13, 18, 21, 19, 22, 11, 12, 14, 17, 9, 13, 3, 2, 10 };

int main ( ) {

    beap<int> a ( std::begin ( data ), std::end ( data ) );

    for ( auto n : data )
        pr ( a, n );

    exit ( 0 );

    // a.insert ( 67 );
    // a.insert ( 9 );
    // a.insert ( 91 );
    // a.insert ( 89 );
    // a.insert ( 19 );

    auto s0 = a.span ( 0 );
    for ( int i = 0; i <= 101; ++i ) {
        std::cout << i << ' ' << a.span ( i ).start << ' ' << a.span ( i ).end << " " << ( ( i * ( i + 1 ) ) / 2 ) << nl;
        auto ns = a.next_span ( s0 );
        auto ps = a.prev_span ( ns );
        assert ( s0.start == ps.start and s0.start == ps.end );
        s0 = ps;
    }

    return EXIT_SUCCESS;
}

#undef ever
