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
#include "mgutility/std/utility.hpp"
#include <initializer_list>
#include <type_traits>
#include <utility>

namespace mgutility {
namespace detail {

#ifndef MGUTILITY_ENUM_RANGE_MIN
/**
 * @brief Defines the MGUTILITY_ENUM_RANGE_MIN macro.
 *
 * This macro defines the minimum value for enum range. Default is 0.
 */
// NOLINTNEXTLINE [cppcoreguidelines-macro-usage]
#define MGUTILITY_ENUM_RANGE_MIN 0
#endif

#ifndef MGUTILITY_ENUM_RANGE_MAX
/**
 * @brief Defines the MGUTILITY_ENUM_RANGE_MAX macro.
 *
 * This macro defines the maximum value for enum range. Default is 256.
 */
// NOLINTNEXTLINE [cppcoreguidelines-macro-usage]
#define MGUTILITY_ENUM_RANGE_MAX 256
#endif

/**
 * @brief Defines the MGUTILITY_ENUM_NAME_BUFFER_SIZE macro.
 *
 * This macro defines the size of the buffer used for enum names.
 */
#ifndef MGUTILITY_ENUM_NAME_BUFFER_SIZE
// NOLINTNEXTLINE [cppcoreguidelines-macro-usage]
#define MGUTILITY_ENUM_NAME_BUFFER_SIZE 128U
#endif

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
      // NOLINTNEXTLINE [modernize-type-traits]
      std::is_enum<E>::value &&
      // NOLINTNEXTLINE [modernize-type-traits]
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
// NOLINTNEXTLINE [modernize-type-traits]
using enable_if_t = typename std::enable_if<B, T>::type;

/**
 * @brief Alias template for std::underlying_type.
 *
 * @tparam T The enumeration type.
 */
template <typename T>
// NOLINTNEXTLINE [modernize-type-traits]
using underlying_type_t = typename std::underlying_type<T>::type;

/**
 * @brief Alias template for std::remove_const.
 *
 * @tparam T The type to remove const from.
 */
template <typename T>
// NOLINTNEXTLINE [modernize-type-traits]
using remove_const_t = typename std::remove_const<T>::type;

/**
 * @brief Represents a sequence of enumeration values.
 *
 * @tparam Enum The enumeration type.
 * @tparam Values The enumeration values.
 */
template <typename Enum, Enum... Values> struct enum_sequence {};

/**
 * @brief Helper for creating enum sequences from index sequences.
 *
 * @tparam Enum The enumeration type.
 * @tparam Min The minimum value.
 * @tparam Seq The index sequence.
 */
template <typename Enum, int Min, typename Seq> struct enum_sequence_from_index;

/**
 * @brief Specialization for creating enum sequences from index sequences.
 *
 * @tparam Enum The enumeration type.
 * @tparam Min The minimum value.
 * @tparam I The indices.
 */
template <typename Enum, int Min, std::size_t... I>
struct enum_sequence_from_index<Enum, Min, index_sequence<I...>> {
private:
  // NOLINTNEXTLINE [readability-identifier-length]
  static constexpr int offset(std::size_t i) {
    return Min + static_cast<int>(i);
  }

public:
  using type = enum_sequence<Enum, static_cast<Enum>(offset(I))...>;
};

/**
 * @brief Alias for creating an enum sequence from a range.
 *
 * @tparam Enum The enumeration type.
 * @tparam Min The minimum value.
 * @tparam Max The maximum value.
 */
template <typename Enum, int Min, int Max>
using make_enum_sequence = typename enum_sequence_from_index<
    Enum, Min,
    make_index_sequence<static_cast<std::size_t>(Max - Min + 1)>>::type;
} // namespace detail

/**
 * @brief Provides the range for an enumeration type.
 *
 * @tparam T The enumeration type.
 */
template <typename T> struct enum_range {
  static constexpr auto min{MGUTILITY_ENUM_RANGE_MIN};
  static constexpr auto max{MGUTILITY_ENUM_RANGE_MAX};
};

template <typename T, typename U> struct pair {
  T first;
  U second;
};

template <typename T>
#if MGUTILITY_CPLUSPLUS >= 201402L || defined(__GNUC__) && !defined(__clang__)
using flat_map = std::initializer_list<pair<T, const char *>>;
#else
// NOLINTNEXTLINE [cppcoreguidelines-avoid-c-arrays]
using flat_map = pair<T, const char *>[];
#endif

/**
 * @brief Provides the custom names map for an enumeration type.
 *
 * @tparam T The enumeration type.
 */
template <typename T> struct custom_enum {
  // #if MGUTILITY_CPLUSPLUS > 201402L
  static constexpr flat_map<T> map = {};
  // #else
  //     static constexpr flat_map<T> map() noexcept {
  //         return {}; // default: empty map
  //     }
  // #endif
};

/**
 * @brief Provides the name buffer size for an enumeration type.
 *
 * @tparam T The enumeration type.
 */
template <typename T> struct enum_name_buffer {
  static constexpr auto size = MGUTILITY_ENUM_NAME_BUFFER_SIZE;
};

} // namespace mgutility

#endif // DETAIL_META_HPP