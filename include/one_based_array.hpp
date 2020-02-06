
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

template<typename ValueType, std::size_t Size>
struct alignas ( std::max ( alignof ( ValueType ), 16ull ) ) one_based_array { // At least 16-byte aligned.

    using value_type             = ValueType;
    using size_type              = std::size_t;
    using difference_type        = std::ptrdiff_t;
    using reference              = value_type &;
    using const_reference        = value_type const &;
    using pointer                = value_type *;
    using const_pointer          = value_type const *;
    using iterator               = value_type *;
    using const_iterator         = value_type const *;
    using reverse_iterator       = value_type *;
    using const_reverse_iterator = value_type const *;

    private:
    using data_type      = std::array<value_type, Size>;
    using std_array_type = data_type;

    public:
    explicit constexpr one_based_array ( ) noexcept = default;

    // one_based_array's.

    constexpr one_based_array ( one_based_array const & other_ ) { copy ( other_ ); }
    constexpr one_based_array ( one_based_array && other_ ) noexcept { move ( other_ ); }

    // Convertible types.

    template<typename U>
    constexpr one_based_array ( one_based_array<U, Size> const & other_ ) {
        copy ( other_ );
    }
    template<typename U>
    constexpr one_based_array ( one_based_array<U, Size> && other_ ) noexcept {
        move ( other_ );
    }

    // std::array's.

    constexpr one_based_array ( std_array_type const & other_ ) { copy ( other_ ); }
    constexpr one_based_array ( std_array_type && other_ ) noexcept { move ( other_ ); }

    // Convertible types.

    template<typename U>
    constexpr one_based_array ( std::array<U, Size> const & other_ ) {
        copy ( other_ );
    }
    template<typename U>
    constexpr one_based_array ( std::array<U, Size> && other_ ) noexcept {
        move ( other_ );
    }

    // std::initializer_list.

    constexpr one_based_array ( std::initializer_list<value_type> list_ ) noexcept {
        std::copy ( std::cbegin ( list_ ), std::cend ( list_ ), begin ( ) );
    }

    // Assignment.

    [[maybe_unused]] constexpr one_based_array & operator= ( one_based_array const & rhs_ ) {
        if ( std::addressof ( rhs_ ) != this )
            copy ( rhs_ );
        return *this;
    }
    [[maybe_unused]] constexpr one_based_array & operator= ( one_based_array && rhs_ ) noexcept {
        if ( std::addressof ( rhs_ ) != this )
            move ( rhs_ );
        return *this;
    }

    // Convertible types.

    template<typename U>
    [[maybe_unused]] constexpr one_based_array & operator= ( one_based_array<U, Size> const & rhs_ ) {
        if ( std::addressof ( rhs_ ) != this )
            copy ( rhs_ );
        return *this;
    }
    template<typename U>
    [[maybe_unused]] constexpr one_based_array & operator= ( one_based_array<U, Size> && rhs_ ) noexcept {
        if ( std::addressof ( rhs_ ) != this )
            move ( rhs_ );
        return *this;
    }

    // Assign from std::array.

    [[maybe_unused]] constexpr one_based_array & operator= ( std_array_type const & rhs_ ) {
        if ( std::addressof ( rhs_ ) != this )
            copy ( rhs_ );
        return *this;
    }
    [[maybe_unused]] constexpr one_based_array & operator= ( std_array_type && rhs_ ) noexcept {
        if ( std::addressof ( rhs_ ) != this )
            move ( rhs_ );
        return *this;
    }

    // Convertible types.

    template<typename U>
    [[maybe_unused]] constexpr one_based_array & operator= ( std::array<U, Size> const & rhs_ ) {
        if ( std::addressof ( rhs_ ) != this )
            copy ( rhs_ );
        return *this;
    }
    template<typename U>
    [[maybe_unused]] constexpr one_based_array & operator= ( std::array<U, Size> && rhs_ ) noexcept {
        if ( std::addressof ( rhs_ ) != this )
            move ( rhs_ );
        return *this;
    }

    // std::initializer_list.

    [[maybe_unused]] constexpr one_based_array & operator= ( std::initializer_list<value_type> list_ ) {
        std::copy ( std::cbegin ( list_ ), std::cend ( list_ ), begin ( ) );
        return *this;
    }

    // data ( ), allows interaction with STL.

    [[nodiscard]] constexpr const_pointer data ( ) const noexcept { return m_data.data ( ); }
    [[nodiscard]] constexpr pointer data ( ) noexcept { return const_cast<pointer> ( std::as_const ( *this ).data ( ) ); }

    // Explicitely access data as zero- or one-based, for use in application that uses b1, or switches between them..

    [[nodiscard]] constexpr const_pointer data_base_zero ( ) const noexcept { return data ( ); }
    [[nodiscard]] constexpr pointer data_base_zero ( ) noexcept {
        return const_cast<pointer> ( std::as_const ( *this ).data_base_zero ( ) );
    }
    [[nodiscard]] constexpr const_pointer data_base_one ( ) const noexcept { return data_base_zero ( ) - 1; }
    [[nodiscard]] constexpr pointer data_base_one ( ) noexcept {
        return const_cast<pointer> ( std::as_const ( *this ).data_base_one ( ) );
    }

    // Iterators.

    public:
    [[nodiscard]] constexpr const_iterator begin ( ) const noexcept { return std::launder ( m_data.begin ( ) ); }
    [[nodiscard]] constexpr const_iterator cbegin ( ) const noexcept { return begin ( ); }
    [[nodiscard]] constexpr iterator begin ( ) noexcept { return const_cast<iterator> ( std::as_const ( *this ).begin ( ) ); }

    [[nodiscard]] constexpr const_iterator end ( ) const noexcept { return std::launder ( m_data.end ( ) ); }
    [[nodiscard]] constexpr const_iterator cend ( ) const noexcept { return end ( ); }
    [[nodiscard]] constexpr iterator end ( ) noexcept { return const_cast<iterator> ( std::as_const ( *this ).end ( ) ); }

    [[nodiscard]] constexpr const_iterator rbegin ( ) const noexcept { return std::launder ( m_data.rbegin ( ) ); }
    [[nodiscard]] constexpr const_iterator crbegin ( ) const noexcept { return rbegin ( ); }
    [[nodiscard]] constexpr iterator rbegin ( ) noexcept { return const_cast<iterator> ( std::as_const ( *this ).rbegin ( ) ); }

    [[nodiscard]] constexpr const_iterator rend ( ) const noexcept { return std::launder ( m_data.rend ( ) ); }
    [[nodiscard]] constexpr const_iterator crend ( ) const noexcept { return rend ( ); }
    [[nodiscard]] constexpr iterator rend ( ) noexcept { return const_cast<iterator> ( std::as_const ( *this ).rend ( ) ); }

    // Access.

    [[nodiscard]] constexpr const_reference front ( ) const noexcept { return m_data.front ( ); }
    [[nodiscard]] constexpr reference front ( ) noexcept { return const_cast<reference> ( std::as_const ( *this ).front ( ) ); }

    [[nodiscard]] constexpr const_reference back ( ) const noexcept { return m_data.back ( ); }
    [[nodiscard]] constexpr reference back ( ) noexcept { return const_cast<reference> ( std::as_const ( *this ).back ( ) ); }

    [[nodiscard]] const_reference at ( size_type const i_ ) const {
        if ( 0 < i_ and i_ <= size ( ) )
            return data_base_one ( )[ i_ ];
        else
            throw std::runtime_error ( "one_based_array: index out of bounds" );
    }
    [[nodiscard]] reference at ( size_type const i_ ) { return const_cast<reference> ( std::as_const ( *this ).at ( i_ ) ); }

    [[nodiscard]] constexpr const_reference operator[] ( size_type const i_ ) const noexcept {
        assert ( 0 < i_ and i_ <= size ( ) );
        return data_base_one ( )[ i_ ];
    }
    [[nodiscard]] constexpr reference operator[] ( size_type const i_ ) noexcept {
        return const_cast<reference> ( std::as_const ( *this ).operator[] ( i_ ) );
    }

    // Sizes.

    [[nodiscard]] static constexpr std::size_t capacity ( ) noexcept { return Size; }
    [[nodiscard]] static constexpr std::size_t size ( ) noexcept { return capacity ( ); } // Non-std (C++17 <), but useful.
    [[nodiscard]] static constexpr std::size_t max_size ( ) noexcept { return capacity ( ); }

    // STL-functionality.

    constexpr void fill ( value_type const & value_ ) { m_data.fill ( value_ ); }
    constexpr void swap ( one_based_array & other_ ) noexcept { m_data.swap ( other_ ); }

    private:
    void memcpy_impl ( void * const to_, void const * const from_, size_t const size_ ) noexcept {
        assert ( to_ and from_ and to_ != from_ and size_ > size_t{ 0u } ); // Check for UB, and sane memcpy usage.
        if ( size_t const size_lower_multiple_of_32 = size_ & 0b1111'1111'1111'1111'1111'1111'1111'0000;
             size_lower_multiple_of_32 ) {
            sax::memcpy_sse_32_impl ( to_, from_, size_lower_multiple_of_32 );
            if ( size_t const size_remaining = size_ - size_lower_multiple_of_32; size_remaining )
                std::memcpy ( to_ + size_lower_multiple_of_32, from_ + size_lower_multiple_of_32, size_remaining );
            return;
        }
        std::memcpy ( tp_, from, size_ );
    }

    template<typename InputIt, typename OutputIt>
    OutputIt copy_impl ( InputIt first_, InputIt last_, OutputIt d_first_ ) {
        if constexpr ( std::is_trivially_copyable<value_type>::value ) {
            return memcpy_impl ( std::launder ( d_first_ ), std::launder ( first_ ), size ( ) * sizeof ( value_type ) );
        }
        else {
            return std::copy ( first_, last_, d_first_ );
        }
    }

    template<typename InputIt, typename OutputIt>
    OutputIt move_impl ( InputIt first_, InputIt last_, OutputIt d_first_ ) {
        if constexpr ( std::is_trivially_copyable<value_type>::value ) {
            return memcpy_impl ( std::launder ( d_first_ ), std::launder ( first_ ), size ( ) * sizeof ( value_type ) );
        }
        else {
            return std::move ( first_, last_, d_first_ );
        }
    }

    public:
    void copy ( one_based_array const & other_ ) {
        copy_impl ( other_.m_data.cbegin ( ), other_.m_data.cend ( ), m_data.begin ( ) );
    }
    template<typename U>
    void copy ( one_based_array<U, Size> const & other_ ) {
        copy_impl ( other_.m_data.cbegin ( ), other_.m_data.cend ( ), m_data.begin ( ) );
    }
    template<typename U>
    void copy ( std::array<U, Size> const & other_ ) {
        copy_impl ( other_.m_data.cbegin ( ), other_.m_data.cend ( ), m_data.begin ( ) );
    }

    void move ( one_based_array && other_ ) noexcept {
        move_impl ( other_.m_data.cbegin ( ), other_.m_data.cend ( ), m_data.begin ( ) );
    }
    template<typename U>
    void move ( one_based_array<U, Size> && other_ ) noexcept {
        move_impl ( other_.m_data.cbegin ( ), other_.m_data.cend ( ), m_data.begin ( ) );
    }
    template<typename U>
    void move ( std::array<U, Size> && other_ ) noexcept {
        move_impl ( other_.m_data.cbegin ( ), other_.m_data.cend ( ), m_data.begin ( ) );
    }

    // Global functions.

    // Comparison.

    [[nodiscard]] constexpr friend bool operator== ( one_based_array const & lhs_, one_based_array const & rhs_ ) noexcept {
        return std::equal ( lhs_.m_data.begin ( ), lhs_.m_data.end ( ), rhs_.m_data.begin ( ) );
    }
    [[nodiscard]] constexpr friend bool operator!= ( one_based_array const & lhs_, one_based_array const & rhs_ ) noexcept {
        return not operator== ( rhs_ );
    }

    [[nodiscard]] constexpr friend bool operator< ( one_based_array const & lhs_, one_based_array const & rhs_ ) noexcept {
        return std::lexicographical_compare ( lhs_.m_data.begin ( ), lhs_.m_data.end ( ), rhs_.m_data.begin ( ),
                                              rhs_.m_data.end ( ), std::less<value_type> ( ) );
    }
    [[nodiscard]] constexpr friend bool operator>= ( one_based_array const & lhs_, one_based_array const & rhs_ ) noexcept {
        return not operator< ( rhs_ );
    }

    [[nodiscard]] constexpr friend bool operator> ( one_based_array const & lhs_, one_based_array const & rhs_ ) noexcept {
        return std::lexicographical_compare ( lhs_.m_data.begin ( ), lhs_.m_data.end ( ), rhs_.m_data.begin ( ),
                                              rhs_.m_data.end ( ), std::greater<value_type> ( ) );
    }
    [[nodiscard]] constexpr friend bool operator<= ( one_based_array const & lhs_, one_based_array const & rhs_ ) noexcept {
        return not operator> ( rhs_ );
    }

    [[nodiscard]] constexpr friend bool operator<=> ( one_based_array const & lhs_, one_based_array const & rhs_ ) noexcept {
        return lexicographical_compare_3way ( lhs_.m_data.begin ( ), lhs_.m_data.end ( ), rhs_.m_data.begin ( ),
                                              rhs_.m_data.end ( ) );
    }

    // Output.

    template<typename Stream>
    [[maybe_unused]] friend Stream & operator<< ( Stream & out_, one_based_array const & m_ ) noexcept {
        std::for_each ( m_.cbegin ( ), m_.cend ( ), [ &out_ ] ( auto & e ) { out_ << e << sp; } );
        return out_;
    }

    data_type m_data;
}; // namespace sax
} // namespace sax
