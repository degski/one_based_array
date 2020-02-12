
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

    private:
    template<size_type S>
    using lookup_table_type = std::array<char, S>;

    public:
    using compare = Compare;

    beap ( ) noexcept        = default;
    beap ( beap const & b_ ) = default;
    beap ( beap && b_ )      = default;

    template<typename ForwardIt>
    beap ( ForwardIt b_, ForwardIt e_ ) : arr ( b_, e_ ), height ( level ( static_cast<size_type> ( e_ - b_ ) ) ) {}

    [[maybe_unused]] beap & operator= ( beap const & b_ ) = default;
    [[maybe_unused]] beap & operator= ( beap && b_ ) = default;

    // The i'th block consists of the i elements stored from position
    // ( i * ( i - 1 ) / 2 + 1 ) through position i * ( i + 1 ) / 2.
    // These formulas use 1 - based i, and return 1 - based array index.
    [[nodiscard]] constexpr span_type span_1_based ( size_type i_ ) const noexcept {
        return { i_ * ( i_ - 1 ) / 2 + 1, i_ * ( i_ + 1 ) / 2 };
    }

    [[nodiscard]] span_type next_span ( span_type span_ ) const noexcept {
        auto tmp = span_.end + 1;
        return { tmp, 2 * tmp - span_.start };
    }
    [[nodiscard]] span_type prev_span ( span_type span_ ) const noexcept {
        return { 2 * span_.start - span_.end, span_.start - 1 };
    }

    [[nodiscard]] size_type next_span_start ( span_type const & span_ ) const noexcept { return span_.end + 1; }
    [[nodiscard]] size_type prev_span_start ( span_type const & span_ ) const noexcept { return 2 * span_.start - span_.end; }

    // Convert to use sane 0-based indexes both for "block" (span)
    // and array.
    [[nodiscard]] constexpr span_type span ( size_type i_ ) const noexcept {
        i_ += 1;
        return { i_ * ( i_ - 1 ) / 2, i_ * ( i_ + 1 ) / 2 - 1 };
    }

    static constexpr size_type invalid = { -1 };

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
        size_type h   = height;
        span_type s   = span ( h );
        size_type idx = s.start;
        for ( ever ) {
            const_reference at_idx = at ( idx );
            if ( bool unequal = v_ != at_idx; unequal ) {
                if ( bool greater = not compare ( ) ( v_, at_idx ); greater ) {
                    // If v_ is less than the element under consideration, move left
                    // 1 position along the row.
                    // These rules are given for weirdly mirrored matrix. They're also
                    // for min beap, we so far implement max beap.
                    // So: if v_ is greater than, and move up along the column.
                    if ( idx == s.end )
                        return { 0, 0 };
                    size_type diff = idx - s.start;
                    h -= 1;
                    idx = prev_span_start ( s ) + diff;
                    continue;
                }
                else {
                    // If v_ exceeds the element, either move down 1 position along the column or if
                    // this is not possible (because we are on the diagonal) then move left and down 1 position
                    // each.
                    // => less, move right along the row, or up and right
                    if ( idx == size ( ) - 1 ) {
                        size_type diff = idx - s.start;
                        h -= 1;
                        idx = prev_span_start ( s ) + diff;
                        continue;
                    }
                    size_type diff     = idx - s.start;
                    span_type new_span = next_span ( s );
                    size_type new_idx  = new_span.start + diff + 1;
                    if ( new_idx < size ( ) ) {
                        h += 1;
                        s   = new_span;
                        idx = new_idx;
                        continue;
                    }
                    if ( idx == s.end )
                        return { 0, 0 };
                    idx += 1;
                    continue;
                }
            }
            else {
                std::cout << "found " << at ( idx ) << nl;
                return { idx, h };
            }
        }
    }

    [[nodiscard]] bool is_idx_eq_end ( size_type idx_ ) const noexcept {
        if ( ( arr.data ( ) + idx_ - 1 ) == std::addressof ( arr.back ( ) ) )
            return true;
        return false;
    }

    [[nodiscard]] span_type search_1 ( value_type const & v_ ) const noexcept {
        size_type h = height;
        span_type s = span ( h );
        size_type i = s.start;
        for ( ever ) {
            std::printf ( "search: idx: %d\n", i );
            if ( bool idx_eq_end = is_idx_eq_end ( i ), is_greater = not idx_eq_end ? v_ > arr[ i ] : true; is_greater ) {
                std::printf ( "moving up ^" );
                if ( idx_eq_end ) {
                    std::printf ( "can't move up\n" );
                    break;
                }
                size_type diff = i - s.start;
                h -= 1;
                s = span ( h );
                i = s.start + diff;
                continue;
            }
            else if ( bool idx_eq_end = is_idx_eq_end ( i ), is_less = not idx_eq_end ? v_ > arr[ i ] : true; is_less ) {
                std::printf ( "moving right ->\n" );
                if ( i == size ( ) - 1 ) {
                    std::printf ( "last element reached, can't move right, moving up instead\n" );
                    size_type diff = i - s.start;
                    h -= 1;
                    s = span ( h );
                    i = s.start + diff;
                    continue;
                }
                size_type diff     = i - s.start;
                span_type new_span = next_span ( s );
                size_type new_idx  = new_span.start + diff + 1;
                if ( new_idx < size ( ) ) {
                    h += 1;
                    s = new_span;
                    i = new_idx;
                    continue;
                }
                std::printf ( "can't move right, moving right-up " );
                if ( idx_eq_end ) {
                    std::printf ( "can't move right-up\n" );
                    break;
                }
                i += 1;
                continue;
            }
            else {
                return { i, h };
            }
        }
        std::printf ( "not found\n" );
        return { 0, 0 };
    }

    [[nodiscard]] span_type search_2 ( value_type const & v_ ) const noexcept {
        size_type h = height;
        span_type s = span ( h );
        size_type i = s.start;
        for ( ever ) {
            std::printf ( "search: idx: %d\n", i );
            if ( v_ > arr[ i ] ) {
                std::printf ( "moving up ^" );
                if ( i == s.end ) {
                    std::printf ( "can't move up\n" );
                    break;
                }
                size_type diff = i - s.start;
                h -= 1;
                s = span ( h );
                i = s.start + diff;
                continue;
            }
            else if ( v_ < arr[ i ] ) {
                std::printf ( "moving right ->\n" );
                if ( i == size ( ) - 1 ) {
                    std::printf ( "last element reached, can't move right, moving up instead\n" );
                    size_type diff = i - s.start;
                    h -= 1;
                    s = span ( h );
                    i = s.start + diff;
                    continue;
                }
                size_type diff     = i - s.start;
                span_type new_span = next_span ( s );
                size_type new_idx  = new_span.start + diff + 1;
                if ( new_idx < size ( ) ) {
                    h += 1;
                    s = new_span;
                    i = new_idx;
                    continue;
                }
                std::printf ( "can't move right, moving right-up " );
                if ( i == s.end ) {
                    std::printf ( "can't move right-up\n" );
                    break;
                }
                i += 1;
                continue;
            }
            else {
                return { i, h };
            }
        }
        std::printf ( "not found\n" );
        return { 0, 0 };
    }

    // Percolate an element up the beap.
    [[nodiscard]] size_type filter_up ( size_type idx_, size_type h_ ) noexcept {
        pointer v = arr.data ( ) + idx_;
        while ( h_ ) {
            auto [ start, end ] = span ( h_ );
            size_type diff = idx_ - start, left_p = 0, right_p = 0, val_l = 0, val_r = 0;
            auto [ st_p, end_p ] = span ( h_ - 1 );
            if ( idx_ != start ) {
                left_p = st_p + diff - 1;
                val_l  = left_p;
            }
            if ( idx_ != end ) {
                right_p = st_p + diff;
                val_r   = right_p;
            }
            if ( val_l != 0 and at ( v ) > at ( val_l ) and ( val_r == 0 or at ( val_l ) < at ( val_r ) ) ) {
                std::swap ( at ( v ), at ( left_p ) );
                idx_ = left_p;
                h_ -= 1;
            }
            else if ( val_r != 0 and at ( v ) > at ( val_r ) ) {
                std::swap ( at ( v ), at ( right_p ) );
                idx_ = right_p;
                h_ -= 1;
            }
            else {
                return idx_;
            }
        }
        assert ( idx_ == 0 );
        return idx_;
    }

    // Percolate an element down the beap.
    [[nodiscard]] size_type filter_down ( size_type idx_, size_type h_ ) noexcept {
        while ( h_ < height - 1 ) {
            auto [ start, end ]  = span ( h_ );
            auto [ st_c, end_c ] = span ( h_ + 1 );
            size_type diff = idx_ - start, left_c = st_c + diff, right_c = 0, val_l = 0, val_r = 0;
            if ( left_c < size ( ) ) {
                val_l   = left_c;
                right_c = left_c + 1;
                if ( right_c >= size ( ) )
                    right_c = 0;
                else
                    val_r = right_c;
            }
            else {
                left_c = 0;
            }
            pointer v = arr.data ( ) + idx_;
            if ( val_l != 0 and at ( v ) < at ( val_l ) and ( val_r == 0 or at ( val_l ) > at ( val_r ) ) ) {
                std::swap ( at ( v ), at ( left_c ) );
                idx_ = left_c;
                h_ += 1;
            }
            else if ( val_r != 0 and at ( v ) < at ( val_r ) ) {
                std::swap ( at ( v ), at ( right_c ) );
                idx_ = right_c;
                h_ += 1;
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
        size_type eos = size ( ) - 1;
        height += static_cast<int> ( eos == end );
        arr.push_back ( v_ );
        std::cout << "filter up " << v_ << " idx " << ( eos + 1 ) << " height " << height << nl;
        return filter_up ( eos + 1, height );
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
    [[nodiscard]] size_type end_of_storage ( ) const noexcept { return static_cast<int> ( arr.size ( ) ) - 1; }

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

    // private:
    [[nodiscard]] const_reference at ( pointer p_ ) const noexcept { return p_[ 0 ]; }
    [[nodiscard]] reference at ( pointer p_ ) noexcept { return p_[ 0 ]; }

    [[nodiscard]] const_reference at ( size_type s_ ) const noexcept {
        // if ( 0 > s_ ) {
        //     std::cout << "negative index used" << nl;
        //     return arr.back ( );
        // }
        return arr.data ( )[ s_ ];
    }
    [[nodiscard]] reference at ( size_type s_ ) noexcept { return std::as_const ( this )->at ( s_ ); }

    value_type && pop ( ) {
        value_type && last = std::move ( arr.back ( ) );
        arr.pop_back ( );
        return std::move ( last );
    }

    value_type check_search ( value_type i_ ) const noexcept {
        auto s = search_2 ( i_ );
        // std::cout << "i " << i_ << " " << s.start << " " << s.end << nl;
        assert ( at ( s.start ) == i_ );
        return s.start;
    }

    // Levels and spans.

    [[nodiscard]] static constexpr size_type level_basic ( size_type n_, size_type l_ = 0, size_type s_ = 0 ) noexcept {
        size_type idx = 1, lvl = 1;
        while ( idx < n_ )
            idx += ( lvl += 1 );
        return lvl;
    }

    [[nodiscard]] static constexpr size_type small_level ( size_type l_ ) noexcept {
        alignas ( 64 ) constexpr lookup_table_type<56> level_small = { 0, 1, 2, 2, 3,  3,  3,  4,  4,  4,  4,  5,  5,  5,
                                                                       5, 5, 6, 6, 6,  6,  6,  6,  7,  7,  7,  7,  7,  7,
                                                                       7, 8, 8, 8, 8,  8,  8,  8,  8,  9,  9,  9,  9,  9,
                                                                       9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10 };
        return level_small[ l_ ];
    }

    template<size_type StartLevel>
    [[nodiscard]] static size_type medium_level ( size_type l_ ) noexcept {
        constexpr size_type sol = start_of_level ( StartLevel ) + 1, l = StartLevel;
        if ( l_ < ( sol + l ) )
            return l;
        if ( l_ < ( sol + 2 * l + 1 ) )
            return l + 1;
        if ( l_ < ( sol + 3 * l + 3 ) )
            return l + 2;
        return l + 3;
    }

    [[nodiscard]] size_type level ( size_type l_ ) const noexcept {
        if ( l_ < start_of_level ( 27 ) ) {
            if ( l_ <= start_of_level ( 11 ) )
                return small_level ( l_ );
            if ( l_ <= start_of_level ( 15 ) )
                return medium_level<11> ( l_ );
            if ( l_ <= start_of_level ( 19 ) )
                return medium_level<15> ( l_ );
            if ( l_ <= start_of_level ( 23 ) )
                return medium_level<19> ( l_ );
            return medium_level<23> ( l_ );
        }
        return level_basic ( l_, start_of_level ( 27 ), level_basic ( start_of_level ( 27 ) ) );
    }

    [[nodiscard]] static constexpr size_type nth_triangular_number ( size_type n_ ) noexcept {
        assert ( n_ >= 0 );
        return ( n_ * ( n_ + 1 ) ) / 2;
    }

    [[nodiscard]] static constexpr size_type start_of_level ( size_type level_ ) noexcept {
        assert ( level_ > 0 );
        return nth_triangular_number ( level_ - 1 );
    }
    [[nodiscard]] static constexpr size_type end_of_level ( size_type level_ ) noexcept {
        assert ( level_ > 0 );
        return nth_triangular_number ( level_ );
    }

    [[nodiscard]] static constexpr span_type level_span ( size_type level_ ) noexcept {
        assert ( level_ > 0 );
        return { start_of_level ( level_ ), end_of_level ( level_ ) };
    }

    [[nodiscard]] static constexpr size_type next_level_span ( size_type level_ ) noexcept {
        assert ( level_ > 0 );
        return level_span ( level_ + 1 );
    }
    [[nodiscard]] static constexpr size_type prev_level_span ( size_type level_ ) noexcept {
        assert ( level_ > 0 );
        return level_span ( level_ - 1 );
    }

    // Members.

    data_type arr;
    size_type height = invalid;
};

template<typename Type, std::size_t Size>
struct triangular_view {

    using value_type = Type;
    using size_type  = int;

    constexpr triangular_view ( ) noexcept                         = default;
    constexpr triangular_view ( triangular_view const & ) noexcept = default;
    constexpr triangular_view ( triangular_view && ) noexcept      = delete;

    [[maybe_unused]] constexpr triangular_view & operator= ( triangular_view const & ) noexcept = default;
    [[maybe_unused]] constexpr triangular_view & operator= ( triangular_view && ) noexcept = delete;

    constexpr triangular_view ( value_type * pointer_ ) noexcept : data ( pointer_ ) {}

    [[nodiscard]] static constexpr size_type size ( ) noexcept { return static_cast<size_type> ( Size ); }

    [[nodiscard]] static constexpr std::size_t nth_triangular_number ( size_type n_ ) noexcept { return n_ * ( ( n_ + 1 ) / 2 ); }

    [[nodiscard]] constexpr value_type const & at ( size_type level_, size_type index_ ) const noexcept {
        return std::addressof ( data[ std::size_t ( 1 ) + nth_triangular_number ( level_ ) ] )[ index_ ];
    }
    [[nodiscard]] constexpr value_type & at ( size_type level_, size_type index_ ) noexcept {
        return std::addressof ( data[ std::size_t ( 1 ) + nth_triangular_number ( level_ ) ] )[ index_ ];
    }

    [[nodiscard]] static constexpr size_type level ( size_type i_ ) noexcept {
        size_type idx = 1, level = 1;
        while ( idx < i_ )
            idx += ( level += 1 );
        return level;
    }

    [[nodiscard]] static constexpr size_type level_begin ( size_type i_ ) noexcept {
        n_ -= 1;
        for ( size_type idx = 1; true; idx += idx + 1 )
            if ( idx > i_ )
                return idx;
        return -1;
    }

    [[nodiscard]] static constexpr sax::pair<size_type, size_type> level_span ( size_type i_ ) noexcept {
        n_ -= 1;
        size_type idx = 1, level = 1;
        while ( idx < n_ )
            idx += ( level += 1 );
        return { idx, idx + level };
    }

    value_type * data = nullptr;
};

template<typename Type, std::size_t Size>
using triangular_array = std::array<Type, triangular_view<int, Size>::size ( )>;

int main ( ) {

    triangular_array<int, 16> a;
    triangular_view<int, 16> tv ( a.data ( ) );

    tv.at ( 2, 4 ) = 25;

    std::cout << tv.at ( 2, 4 ) << nl;

    return EXIT_SUCCESS;
}

// Data from Ian Munro's "ImpSODA06.ppt" presentation, Slide 3,
// with mistake corrected( 21 and 22 not in beap order ).
std::array<int, 24> data = { 72, 68, 63, 44, 62, 55, 33, 22, 32, 51, 13, 18, 21, 19, 22, 11, 12, 14, 17, 9, 13, 3, 2, 10 };

int main78678 ( ) {

    beap<int> a ( std::begin ( data ), std::end ( data ) );

    // Existing elements.

    /*
    a.check_search ( 72 );
    a.check_search ( 68 );
    a.check_search ( 63 );
    a.check_search ( 44 );
    a.check_search ( 62 );
    a.check_search ( 55 );
    a.check_search ( 33 );
    a.check_search ( 22 );
    a.check_search ( 51 );
    a.check_search ( 13 );
    a.check_search ( 18 );
    a.check_search ( 21 );
    a.check_search ( 19 );
    a.check_search ( 22 );
    a.check_search ( 11 );
    a.check_search ( 12 );
    a.check_search ( 14 );
    a.check_search ( 17 );
    a.check_search ( 9 );
    a.check_search ( 13 );
    a.check_search ( 3 );
    a.check_search ( 2 );
    a.check_search ( 10 );

    std::cout << "found all contained items" << nl;
    */

    // Non-existing elements.

    // a.check_search ( 44 );

    // for ( int i = 0; i < 64; ++i )
    //  std::cout << i << ' ' << a.span ( i ).start << ' ' << a.span ( i ).end << nl;
    /*
    for ( int i = 1; i < 1'200; ++i )
        std::cout << std::setw ( 4 ) << std::setfill ( ' ' ) << i << ' ' << std::setw ( 6 ) << std::setfill ( ' ' )
                  << a.start_of_level ( i ) << ' ' << std::setw ( 6 ) << std::setfill ( ' ' ) << a.end_of_level ( i )
                  << std::setw ( 2 ) << nl;

    exit ( 0 );
    */
    for ( int i = 0; i < 1'200; ++i )
        std::cout << std::setw ( 4 ) << i << ' ' << std::setw ( 2 ) << a.level_basic ( i ) << ' ' << a.level ( i ) << ' '
                  << std::setw ( 0 ) << std::boolalpha << ( a.level_basic ( i ) == a.level ( i ) ) << std::noboolalpha << nl;
    exit ( 0 );

    for ( int const & n : data ) {
        if ( int const r = a.check_search ( n ); r != n ) {
            std::cout << "did not pass.";
            break;
        }
    }

    //  for ( int i = 2; i < 73; ++i )
    //    pr ( a, i );

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
#undef LEVEL_DISPATCH_BLOCK
