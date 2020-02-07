
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

#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include <array>
#include <sax/iostream.hpp>
#include <initializer_list>
#include <limits>
#include <type_traits>

#include <sax/stl.hpp>

namespace sax {

struct compare_3way {
    template<typename T, typename U>
    [[nodiscard]] constexpr int operator( ) ( T const & lhs_, U const & rhs_ ) const noexcept {
        if ( lhs_ < rhs_ )
            return -1;
        if ( rhs_ < lhs_ )
            return 1;
        return 0;
    }
};

template<typename InputIt1, typename InputIt2, typename Compare = compare_3way>
[[nodiscard]] constexpr int lexicographical_compare_3way ( InputIt1 first1_, InputIt1 last1_, InputIt2 first2_,
                                                           InputIt2 last2_ ) noexcept {
    for ( ; ( first1_ != last1_ ) && ( first2_ != last2_ ); ++first1_, ( void ) ++first2_ )
        if ( int c = Compare ( *first1_, *first2_ ) )
            return c;
    return first1_ == last1_ ? ( ( int ) ( first2_ == last2_ ) - 1 ) : 1;
}

template<typename ValueType, std::size_t Size, std::size_t SSEThreshold = 48ull>
struct alignas ( ( sizeof ( ValueType ) * Size ) >= SSEThreshold ? std::max ( alignof ( ValueType ), 16ull )
                                                                 : alignof ( ValueType ) ) based_array {
    private:
    using data_type      = std::array<ValueType, Size>;
    using std_array_type = data_type;

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

    explicit based_array ( ) noexcept = default;

    // based_array's.

    based_array ( based_array const & other_ ) { copy ( other_ ); }
    based_array ( based_array && other_ ) noexcept { move ( std::move ( other_ ) ); }

    // Convertible types.

    template<typename U>
    based_array ( based_array<U, Size> const & other_ ) {
        copy ( other_ );
    }
    template<typename U>
    based_array ( based_array<U, Size> && other_ ) noexcept {
        move ( std::move ( other_ ) );
    }

    // std::array's.

    based_array ( std_array_type const & other_ ) { copy ( other_ ); }
    based_array ( std_array_type && other_ ) noexcept { move ( std::move ( other_ ) ); }

    // Convertible types.

    template<typename U>
    based_array ( std::array<U, Size> const & other_ ) {
        copy ( other_ );
    }
    template<typename U>
    based_array ( std::array<U, Size> && other_ ) noexcept {
        move ( std::move ( other_ ) );
    }

    // std::initializer_list.

    based_array ( std::initializer_list<value_type> list_ ) noexcept {
        std::copy ( std::cbegin ( list_ ), std::cend ( list_ ), begin ( ) );
    }

    // Assignment.

    [[maybe_unused]] based_array & operator= ( based_array const & rhs_ ) {
        if ( std::addressof ( rhs_ ) != this )
            copy ( rhs_ );
        return *this;
    }
    [[maybe_unused]] based_array & operator= ( based_array && rhs_ ) noexcept {
        if ( std::addressof ( rhs_ ) != this )
            move ( std::move ( rhs_ ) );
        return *this;
    }

    // Convertible types.

    template<typename U>
    [[maybe_unused]] based_array & operator= ( based_array<U, Size> const & rhs_ ) {
        if ( std::addressof ( rhs_ ) != this )
            copy ( rhs_ );
        return *this;
    }
    template<typename U>
    [[maybe_unused]] based_array & operator= ( based_array<U, Size> && rhs_ ) noexcept {
        if ( std::addressof ( rhs_ ) != this )
            move ( std::move ( rhs_ ) );
        return *this;
    }

    // Assign from std::array.

    [[maybe_unused]] based_array & operator= ( std_array_type const & rhs_ ) {
        if ( std::addressof ( rhs_ ) != this )
            copy ( rhs_ );
        return *this;
    }
    [[maybe_unused]] based_array & operator= ( std_array_type && rhs_ ) noexcept {
        if ( std::addressof ( rhs_ ) != this )
            move ( std::move ( rhs_ ) );
        return *this;
    }

    // Convertible types.

    template<typename U>
    [[maybe_unused]] based_array & operator= ( std::array<U, Size> const & rhs_ ) {
        if ( std::addressof ( rhs_ ) != this )
            copy ( rhs_ );
        return *this;
    }
    template<typename U>
    [[maybe_unused]] based_array & operator= ( std::array<U, Size> && rhs_ ) noexcept {
        if ( std::addressof ( rhs_ ) != this )
            move ( std::move ( rhs_ ) );
        return *this;
    }

    // std::initializer_list.

    [[maybe_unused]] based_array & operator= ( std::initializer_list<value_type> list_ ) {
        std::copy ( std::cbegin ( list_ ), std::cend ( list_ ), begin ( ) );
        return *this;
    }

    // data ( ), allows interaction with STL.

    [[nodiscard]] constexpr const_pointer data ( ) const noexcept { return m_data.data ( ); }
    [[nodiscard]] constexpr pointer data ( ) noexcept { return m_data.data ( ); }

    // Explicitely access data as zero- or one-based, for use in application that uses b1, or switches between them..

    private:
    template<difference_type Base>
    [[nodiscard]] constexpr const_pointer data_base ( ) const noexcept {
        return data ( ) - Base;
    }
    template<difference_type Base>
    [[nodiscard]] constexpr pointer data_base ( ) noexcept {
        return data ( ) - Base;
    }

    // Iterators.

    public:
    [[nodiscard]] iterator begin ( ) noexcept { return m_data.begin ( ); }
    [[nodiscard]] const_iterator cbegin ( ) const noexcept { return m_data.begin ( ); }

    [[nodiscard]] iterator end ( ) noexcept { return m_data.end ( ); }
    [[nodiscard]] const_iterator cend ( ) const noexcept { return m_data.end ( ); }

    [[nodiscard]] iterator rbegin ( ) noexcept { return m_data.rbegin ( ); }
    [[nodiscard]] const_iterator crbegin ( ) const noexcept { return m_data.rbegin ( ); }

    [[nodiscard]] iterator rend ( ) noexcept { return m_data.rend ( ); }
    [[nodiscard]] const_iterator crend ( ) const noexcept { return m_data.rend ( ); }

    // Access.

    [[nodiscard]] reference front ( ) noexcept { return m_data.front ( ); }
    [[nodiscard]] const_reference front ( ) const noexcept { return m_data.front ( ); }

    [[nodiscard]] reference back ( ) noexcept { return m_data.back ( ); }
    [[nodiscard]] const_reference back ( ) const noexcept { return m_data.back ( ); }

    template<difference_type Base>
    [[nodiscard]] const_reference at ( size_type const i_ ) const {
        if ( Base < i_ and i_ <= Base + size ( ) )
            return get<Base> ( i_ );
        else
            throw std::runtime_error ( "based_array: index out of bounds" );
    }
    template<difference_type Base>
    [[nodiscard]] reference at ( size_type const i_ ) {
        return const_cast<reference> ( std::as_const ( *this ).at<Base> ( i_ ) );
    }

    // Subscript operator, only in base = 0.
    [[nodiscard]] constexpr const_reference operator[] ( size_type const i_ ) const noexcept { return m_data[ i_ ]; }
    [[nodiscard]] constexpr reference operator[] ( size_type const i_ ) noexcept { return m_data[ i_ ]; }

    template<difference_type Base>
    [[nodiscard]] constexpr const_reference get ( size_type const i_ ) const noexcept {
        assert ( Base < i_ and i_ <= Base + size ( ) );
        return data_base<Base> ( )[ i_ ];
    }
    template<difference_type Base>
    [[nodiscard]] constexpr reference get ( size_type const i_ ) noexcept {
        assert ( Base < i_ and i_ <= Base + size ( ) );
        return data_base<Base> ( )[ i_ ];
    }

    // Sizes.

    [[nodiscard]] static constexpr size_type capacity ( ) noexcept { return Size; }
    [[nodiscard]] static constexpr size_type size ( ) noexcept { return capacity ( ); } // Non-std (C++17 <), but useful.
    [[nodiscard]] static constexpr size_type max_size ( ) noexcept { return capacity ( ); }

    // STL-functionality.

    void fill ( value_type const & value_ ) { m_data.fill ( value_ ); }
    void swap ( based_array & other_ ) noexcept { m_data.swap ( other_ ); }

    private:
    void memcpy_impl ( std::byte * to_, std::byte const * from_ ) noexcept {
        constexpr size_type const zero = 0ull;
        assert ( to_ and from_ and to_ != from_ ); // Check for UB.
        if constexpr ( constexpr size_type const size_lower_multiple_of_16 =
                           sizeof ( based_array ) & 0b1111'1111'1111'1111'1111'1111'1111'0000;
                       size_lower_multiple_of_16 != zero ) {
            if constexpr ( ( size_lower_multiple_of_16 & 0b0000'0000'0000'0000'0000'0000'0001'0000 ) != zero ) {
                memcpy_sse_16_impl ( to_, from_ );
                if constexpr ( ( size_lower_multiple_of_16 & 0b1111'1111'1111'1111'1111'1111'1110'0000 ) != zero )
                    memcpy_sse_32_impl ( to_ + 16ll, from_ + 16ll, size_lower_multiple_of_16 - 16ll );
            }
            else {
                memcpy_sse_32_impl ( to_, from_, size_lower_multiple_of_16 );
            }
            if constexpr ( constexpr size_type const size_remaining = sizeof ( based_array ) - size_lower_multiple_of_16;
                           size_remaining != zero )
                std::memcpy ( to_ + size_lower_multiple_of_16, from_ + size_lower_multiple_of_16, size_remaining );
            return;
        }
        else {
            std::memcpy ( to_, from_, sizeof ( based_array ) );
        }
    }

    std::byte const * byte_addressof ( const_iterator it_ ) const noexcept {
        return reinterpret_cast<std::byte const *> ( std::addressof ( *it_ ) );
    }

    void copy_impl ( const_iterator begin_, const_iterator end_ ) {
        if constexpr ( std::is_trivially_copyable<value_type>::value and sizeof ( based_array ) >= SSEThreshold )
            memcpy_impl ( reinterpret_cast<std::byte *> ( m_data.data ( ) ), byte_addressof ( begin_ ) );
        else
            std::copy ( begin_, end_, m_data.begin ( ) );
    }

    void move_impl ( const_iterator begin_, const_iterator end_ ) {
        if constexpr ( std::is_trivially_copyable<value_type>::value and sizeof ( based_array ) >= SSEThreshold )
            memcpy_impl ( reinterpret_cast<std::byte *> ( m_data.data ( ) ), byte_addressof ( begin_ ) );
        else
            std::move ( begin_, end_, m_data.begin ( ) );
    }

    public:
    void copy ( based_array const & other_ ) { copy_impl ( other_.cbegin ( ), other_.cend ( ) ); }
    template<typename U>
    void copy ( based_array<U, Size> const & other_ ) {
        copy_impl ( other_.cbegin ( ), other_.cend ( ) );
    }
    template<typename U>
    void copy ( std::array<U, Size> const & other_ ) {
        copy_impl ( other_.cbegin ( ), other_.cend ( ) );
    }

    void move ( based_array && other_ ) noexcept { move_impl ( other_.cbegin ( ), other_.cend ( ) ); }
    template<typename U>
    void move ( based_array<U, Size> && other_ ) noexcept {
        move_impl ( other_.cbegin ( ), other_.cend ( ) );
    }
    template<typename U>
    void move ( std::array<U, Size> && other_ ) noexcept {
        move_impl ( other_.cbegin ( ), other_.cend ( ) );
    }

    // Global functions.

    // Comparison.

    [[nodiscard]] constexpr friend bool operator== ( based_array const & lhs_, based_array const & rhs_ ) noexcept {
        return std::equal ( lhs_.m_data.begin ( ), lhs_.m_data.end ( ), rhs_.m_data.begin ( ) );
    }
    [[nodiscard]] constexpr friend bool operator!= ( based_array const & lhs_, based_array const & rhs_ ) noexcept {
        return not operator== ( rhs_ );
    }

    [[nodiscard]] constexpr friend bool operator< ( based_array const & lhs_, based_array const & rhs_ ) noexcept {
        return std::lexicographical_compare ( lhs_.m_data.begin ( ), lhs_.m_data.end ( ), rhs_.m_data.begin ( ),
                                              rhs_.m_data.end ( ), std::less<value_type> ( ) );
    }
    [[nodiscard]] constexpr friend bool operator>= ( based_array const & lhs_, based_array const & rhs_ ) noexcept {
        return not operator< ( rhs_ );
    }

    [[nodiscard]] constexpr friend bool operator> ( based_array const & lhs_, based_array const & rhs_ ) noexcept {
        return std::lexicographical_compare ( lhs_.m_data.begin ( ), lhs_.m_data.end ( ), rhs_.m_data.begin ( ),
                                              rhs_.m_data.end ( ), std::greater<value_type> ( ) );
    }
    [[nodiscard]] constexpr friend bool operator<= ( based_array const & lhs_, based_array const & rhs_ ) noexcept {
        return not operator> ( rhs_ );
    }

    [[nodiscard]] constexpr friend bool operator<=> ( based_array const & lhs_, based_array const & rhs_ ) noexcept {
        return lexicographical_compare_3way ( lhs_.m_data.begin ( ), lhs_.m_data.end ( ), rhs_.m_data.begin ( ),
                                              rhs_.m_data.end ( ) );
    }

    // Output.

    template<typename Stream>
    [[maybe_unused]] friend Stream & operator<< ( Stream & out_, based_array const & m_ ) noexcept {
        std::for_each ( m_.cbegin ( ), m_.cend ( ), [ &out_ ] ( auto & e ) { out_ << e << sp; } );
        return out_;
    }

    data_type m_data;
};
} // namespace sax
