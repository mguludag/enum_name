/*
MIT License

Copyright (c) 2024 mguludag

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef DETAIL_META_HPP
#define DETAIL_META_HPP

#include "mgutility/_common/definitions.hpp"
#include <type_traits>

namespace mgutility {
namespace detail {
/**
 * @brief Trait to check if a type is a scoped enumeration.
 *
 * @tparam E The type to check.
 */
template <typename E> struct is_scoped_enum {
  /**
   * @brief Boolean value indicating if the type is a scoped enumeration.
   */
  static constexpr auto value =
      std::is_enum<E>::value &&
      !std::is_convertible<E, typename std::underlying_type<E>::type>::value;
};

/**
 * @brief Trait to check if a type supports the bitwise OR operator.
 *
 * @tparam T The type to check.
 * @tparam Enable SFINAE parameter, default is void.
 */
template <typename T, typename = void> struct has_bit_or : std::false_type {};

/**
 * @brief Specialization of has_bit_or for types that support the bitwise OR
 * operator.
 *
 * @tparam T The type to check.
 */
template <typename T>
struct has_bit_or<T, decltype((T{} | T{}), void())> : std::true_type {};

#if MGUTILITY_CPLUSPLUS > 201103L
/**
 * @brief Helper variable template for is_scoped_enum.
 *
 * @tparam E The type to check.
 */
template <typename E>
static constexpr bool is_scoped_enum_v = is_scoped_enum<E>::value;
#endif

/**
 * @brief Alias template for std::enable_if.
 *
 * This template is used to conditionally enable a type `T` if the boolean
 * constant `B` is true. It is a shorthand for `typename std::enable_if<B,
 * T>::type`.
 *
 * @tparam B The compile-time boolean condition.
 * @tparam T The type to be enabled if `B` is true, default is void.
 */
template <bool B, class T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

/**
 * @brief Alias template for std::underlying_type.
 *
 * @tparam T The enumeration type.
 */
template <typename T>
using underlying_type_t = typename std::underlying_type<T>::type;

/**
 * @brief Alias template for std::remove_const.
 *
 * @tparam T The type to remove const from.
 */
template <typename T>
using remove_const_t = typename std::remove_const<T>::type;

/**
 * @brief Represents a sequence of enumeration values.
 *
 * @tparam Enum The enumeration type.
 * @tparam ...EnumValues The enumeration values in the sequence.
 */
template <typename Enum, Enum...> struct enum_sequence {};

/**
 * @brief Helper for generating a sequence of enumeration values.
 *
 * @tparam Enum The enumeration type.
 * @tparam Min The minimum value in the sequence.
 * @tparam Max The maximum value in the sequence.
 * @tparam ...Next The next values in the sequence.
 */
template <typename Enum, int Min, int Max, int... Next>
struct enum_sequence_helper
    : enum_sequence_helper<Enum, Min, Max - 1, Max - 1, Next...> {};

/**
 * @brief Specialization of enum_sequence_helper for the end of the sequence.
 *
 * @tparam Enum The enumeration type.
 * @tparam Min The minimum value in the sequence.
 * @tparam ...Next The next values in the sequence.
 */
template <typename Enum, int Min, int... Next>
struct enum_sequence_helper<Enum, Min, Min, Next...> {
  /**
   * @brief The resulting sequence type.
   */
  using type = enum_sequence<Enum, static_cast<Enum>(Next)...>;
};

/**
 * @brief Generates a sequence of enumeration values.
 *
 * @tparam Enum The enumeration type.
 * @tparam Min The minimum value in the sequence.
 * @tparam Max The maximum value in the sequence.
 */
template <typename Enum, int Min, int Max>
using make_enum_sequence = typename enum_sequence_helper<Enum, Min, Max>::type;
} // namespace detail

/**
 * @brief Provides the range for an enumeration type.
 *
 * @tparam T The enumeration type.
 */
template <typename T> struct enum_range {
  static constexpr auto min{0};
  static constexpr auto max{256};
};
} // namespace mgutility

#endif // DETAIL_META_HPP