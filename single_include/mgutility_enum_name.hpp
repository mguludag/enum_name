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

#ifndef MGUTILITY_COMMON_DEFINITIONS_HPP
#define MGUTILITY_COMMON_DEFINITIONS_HPP

/**
 * @file definitions.hpp
 * @brief Defines macros for compiler and standard support detection.
 */

/**
 * @brief Defines the MGUTILITY_CPLUSPLUS macro for MSVC and other compilers.
 *
 * For MSVC, it uses _MSVC_LANG. For other compilers, it uses __cplusplus.
 */
#ifdef _MSC_VER
#ifndef MGUTILITY_CPLUSPLUS
#define MGUTILITY_CPLUSPLUS _MSVC_LANG
#endif
#else
#ifndef MGUTILITY_CPLUSPLUS
#define MGUTILITY_CPLUSPLUS __cplusplus
#endif
#endif

/**
 * @brief Defines the MGUTILITY_CNSTXPR macro based on the C++ standard.
 *
 * If the C++ standard is C++11, MGUTILITY_CNSTXPR is defined as empty.
 * If the C++ standard is newer than C++11, MGUTILITY_CNSTXPR is defined as
 * constexpr. If the C++ standard is older than C++11, an error is raised.
 */
#if MGUTILITY_CPLUSPLUS == 201103L
#define MGUTILITY_CNSTXPR
#elif MGUTILITY_CPLUSPLUS > 201103L
#define MGUTILITY_CNSTXPR constexpr
#elif MGUTILITY_CPLUSPLUS < 201103L
#error "Standards older than C++11 is not supported!"
#endif

/**
 * @brief Defines the MGUTILITY_CNSTXPR_CLANG_WA macro based on the C++
 * standard.
 *
 * If the C++ standard is newer than C++17 and the compiler is not Clang,
 * MGUTILITY_CNSTXPR_CLANG_WA is defined as constexpr. Otherwise, it is defined
 * as empty.
 */
#if (MGUTILITY_CPLUSPLUS >= 201703L && !defined(__clang__)) ||                 \
    (defined(__clang__) && __clang_major__ > 11 &&                             \
     MGUTILITY_CPLUSPLUS >= 201703L)
#define MGUTILITY_CNSTXPR_CLANG_WA constexpr
#else
#define MGUTILITY_CNSTXPR_CLANG_WA
#endif

/**
 * @brief Defines the MGUTILITY_CNSTEVL macro based on the C++ standard.
 *
 * If the C++ standard is newer than C++17, MGUTILITY_CNSTEVL is defined as
 * consteval. Otherwise, it is defined as empty.
 */
#if MGUTILITY_CPLUSPLUS > 201703L
#define MGUTILITY_CNSTEVL consteval
#else
#define MGUTILITY_CNSTEVL
#endif

/**
 * @brief Defines the MGUTILITY_HAS_HAS_INCLUDE macro if __has_include is
 * supported.
 *
 * If __has_include is supported, MGUTILITY_HAS_HAS_INCLUDE is defined.
 */
#if defined(__has_include) && !defined(MGUTILITY_HAS_HAS_INCLUDE)
#define MGUTILITY_HAS_HAS_INCLUDE
#endif

#endif // MGUTILITY_COMMON_DEFINITIONS_HPP

#ifndef DETAIL_META_HPP
#define DETAIL_META_HPP

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
 * @brief Represents a compile-time sequence of indices.
 *
 * @tparam Ints The sequence of indices.
 */
template <std::size_t... Ints> struct index_sequence {};

/**
 * @brief Concatenates two index sequences.
 *
 * @tparam Seq1 The first index sequence.
 * @tparam Seq2 The second index sequence.
 */
template <typename Seq1, typename Seq2> struct index_sequence_concat;

template <std::size_t... I1, std::size_t... I2>
struct index_sequence_concat<index_sequence<I1...>, index_sequence<I2...>> {
  using type = index_sequence<I1..., (sizeof...(I1) + I2)...>;
};

/**
 * @brief Implementation helper for creating index sequences.
 *
 * @tparam N The size of the index sequence to create.
 */
template <std::size_t N> struct make_index_sequence_impl;

template <std::size_t N> struct make_index_sequence_impl {
private:
  static constexpr std::size_t half = N / 2;

  using first = typename make_index_sequence_impl<half>::type;
  using second = typename make_index_sequence_impl<N - half>::type;

public:
  using type = typename index_sequence_concat<first, second>::type;
};

// base cases
/**
 * @brief Base case for index sequence of size 0.
 */
template <> struct make_index_sequence_impl<0> {
  using type = index_sequence<>;
};

/**
 * @brief Base case for index sequence of size 1.
 */
template <> struct make_index_sequence_impl<1> {
  using type = index_sequence<0>;
};

/**
 * @brief Alias for creating an index sequence of size N.
 *
 * @tparam N The size of the index sequence.
 */
template <std::size_t N>
using make_index_sequence = typename make_index_sequence_impl<N>::type;

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
#if MGUTILITY_CPLUSPLUS > 201402L || defined(__GNUC__) && !defined(__clang__)
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
  //   static constexpr flat_map<T> map() noexcept {
  //     return {}; // default: empty map
  //   }
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

#ifndef MGUTILITY_STRING_VIEW_HPP
#define MGUTILITY_STRING_VIEW_HPP

#include <cstring>
// NOLINTNEXTLINE [unused-includes]
#include <iosfwd>
#include <string>
#include <utility>

#if MGUTILITY_CPLUSPLUS > 201402L
#include <string_view>
#endif

namespace mgutility {
namespace detail {

/**
 * @brief Computes the length of a C-string at compile-time.
 *
 * @param str The C-string.
 * @param sz The initial size, default is 0.
 * @return The length of the C-string.
 */
// NOLINTNEXTLINE [readability-identifier-length]
constexpr auto strlen_constexpr(const char *str, size_t sz = 0) noexcept
    -> size_t {
  // NOLINTNEXTLINE [cppcoreguidelines-pro-bounds-pointer-arithmetic]
  return str[sz] == '\0' ? sz : strlen_constexpr(str, ++sz);
}

/**
 * @brief Checks if a character is a digit.
 *
 * @param character The character to check.
 * @return True if the character is a digit, otherwise false.
 */
constexpr auto is_digit(char character) noexcept -> bool {
  return character >= '0' && character <= '9';
}

/**
 * @brief Compares two C-strings up to a given number of characters.
 *
 * @param lhs The left-hand side C-string.
 * @param rhs The right-hand side C-string.
 * @param count The maximum number of characters to compare.
 * @return An integer less than, equal to, or greater than zero if lhs is found,
 *         respectively, to be less than, to match, or be greater than rhs.
 */
MGUTILITY_CNSTXPR int strncmp_constexpr(const char *lhs, const char *rhs,
                                        size_t count) noexcept {
  for (size_t i = 0; i < count; ++i) {
    // NOLINTNEXTLINE [cppcoreguidelines-pro-bounds-pointer-arithmetic]
    if (lhs[i] != rhs[i] || lhs[i] == '\0' || rhs[i] == '\0') {
      // NOLINTNEXTLINE [cppcoreguidelines-pro-bounds-pointer-arithmetic]
      return static_cast<unsigned char>(lhs[i]) -
             // NOLINTNEXTLINE [cppcoreguidelines-pro-bounds-pointer-arithmetic]
             static_cast<unsigned char>(rhs[i]);
    }
  }
  return 0;
}

template <typename Range, typename Pred>
MGUTILITY_CNSTXPR auto find(const Range &rng, const Pred &pred) -> size_t {
  for (size_t i = 1; i < rng.size(); ++i) {
    if (pred == rng[i]) {
      return i;
    }
  }
  return 0;
};

} // namespace detail

#if MGUTILITY_CPLUSPLUS < 201703L

/**
 * @brief A basic string view class template.
 *
 * @tparam Char The character type, default is char.
 */
// NOLINTNEXTLINE [cppcoreguidelines-special-member-functions]
template <typename Char = char> class basic_string_view {
public:
  /**
   * @brief Default constructor.
   */
  // NOLINTNEXTLINE [readability-redundant-inline-specifier]
  constexpr inline basic_string_view() noexcept : data_(""), size_(0) {}

  /**
   * @brief Constructs a basic_string_view from a C-string.
   *
   * @param str The C-string.
   */
  // NOLINTNEXTLINE [google-explicit-constructor]
  constexpr inline basic_string_view(const Char *str) noexcept
      : data_(str), size_(detail::strlen_constexpr(str)) {}

  /**
   * @brief Constructs a basic_string_view from a std::string.
   *
   * @param str The std::string.
   */
  // NOLINTNEXTLINE [google-explicit-constructor]
  constexpr basic_string_view(const std::basic_string<Char> &str) noexcept
      : data_(str.c_str()), size_(str.size()) {}

  /**
   * @brief Constructs a basic_string_view from a C-string and length.
   *
   * @param str The C-string.
   * @param len The length of the string.
   */
  constexpr basic_string_view(const Char *str, size_t len) noexcept
      : data_(str), size_(len) {}

  /**
   * @brief Copy constructor.
   *
   * @param other The other basic_string_view to copy.
   */
  constexpr basic_string_view(const basic_string_view &other)
      : data_(other.data_), size_(other.size_) {}

  /**
   * @brief Move constructor.
   *
   * @param other The other basic_string_view to move.
   */
  constexpr basic_string_view(basic_string_view &&other) noexcept
      : data_(std::move(other.data_)), size_(other.size_) {}

  /**
   * @brief Copy assignment operator.
   *
   * @param other The other basic_string_view to copy.
   * @return A reference to this object.
   */
  MGUTILITY_CNSTXPR inline basic_string_view<Char> &
  operator=(const basic_string_view &other) noexcept = default;

  /**
   * @brief Move assignment operator.
   *
   * @param other The other basic_string_view to move.
   * @return A reference to this object.
   */
  MGUTILITY_CNSTXPR inline basic_string_view<Char> &
  operator=(basic_string_view &&other) noexcept {
    data_ = std::move(other.data_);
    size_ = other.size_;
    return *this;
  }

  /**
   * @brief Accesses the character at the given index.
   *
   * @param index The index.
   * @return The character at the index.
   */
  // NOLINTNEXTLINE [readability-const-return-type]
  constexpr const Char operator[](size_t index) const noexcept {
    // NOLINTNEXTLINE [cppcoreguidelines-pro-bounds-pointer-arithmetic]
    return data_[index];
  }

  /**
   * @brief Returns an iterator to the beginning of the string.
   *
   * @return A pointer to the first character.
   */
  constexpr const Char *begin() const noexcept { return data_; }

  /**
   * @brief Returns an iterator to the end of the string.
   *
   * @return A pointer to one past the last character.
   */
  constexpr const Char *end() const noexcept { return (data_ + size_); }

  /**
   * @brief Checks if the string is empty.
   *
   * @return True if the string is empty, otherwise false.
   */
  constexpr bool empty() const noexcept { return size_ < 1; }

  /**
   * @brief Returns the size of the string.
   *
   * @return The size of the string.
   */
  constexpr size_t size() const noexcept { return size_; }

  /**
   * @brief Returns a pointer to the underlying data.
   *
   * @return A pointer to the data.
   */
  constexpr const Char *data() const noexcept { return data_; }

  /**
   * @brief Returns a substring view.
   *
   * @param begin The starting position.
   * @param len The length of the substring.
   * @return A basic_string_view representing the substring.
   */
  constexpr basic_string_view<Char> substr(size_t begin,
                                           size_t len = 0U) const noexcept {
    return basic_string_view<Char>(data_ + begin,
                                   len == 0U ? size_ - begin : len);
  }

  /**
   * @brief Finds the last occurrence of a character.
   *
   * @param c The character to find.
   * @param pos The position to start from, default is npos.
   * @return The position of the character or npos if not found.
   */
  // NOLINTNEXTLINE [readability-identifier-length]
  constexpr size_t rfind(Char c, size_t pos = npos) const noexcept {
    // NOLINTNEXTLINE [cppcoreguidelines-pro-bounds-pointer-arithmetic]
    return (pos == npos ? pos = size_ : pos = pos),
           c == data_[pos] ? pos
           // NOLINTNEXTLINE [readability-avoid-nested-conditional-operator]
           : pos == 0U ? npos
                       : rfind(c, --pos);
  }

  /**
   * @brief Finds the first occurrence of a character.
   *
   * @param c The character to find.
   * @param pos The position to start from, default is 0.
   * @return The position of the character or npos if not found.
   */
  // NOLINTNEXTLINE [readability-identifier-length]
  constexpr size_t find(Char c, size_t pos = 0) const noexcept {
    // NOLINTNEXTLINE [readability-avoid-nested-conditional-operator]
    return c == data_[pos] ? pos : pos < size_ ? find(c, ++pos) : npos;
  }

  /**
   * @brief Equality operator.
   *
   * @param lhs The left-hand side basic_string_view.
   * @param rhs The right-hand side basic_string_view.
   * @return True if the strings are equal, otherwise false.
   */
  MGUTILITY_CNSTXPR friend bool
  operator==(basic_string_view<Char> lhs,
             basic_string_view<Char> rhs) noexcept {
    return (lhs.size_ == rhs.size_) &&
           detail::strncmp_constexpr(lhs.data_, rhs.data_, lhs.size_) == 0;
  }

  /**
   * @brief Equality operator.
   *
   * @param lhs The left-hand side basic_string_view.
   * @param rhs The right-hand side C-string.
   * @return True if the strings are equal, otherwise false.
   */
  MGUTILITY_CNSTXPR friend bool operator==(basic_string_view<Char> lhs,
                                           const Char *rhs) noexcept {
    return ((lhs.size_ == detail::strlen_constexpr(rhs)) &&
            (detail::strncmp_constexpr(lhs.data_, rhs, lhs.size_) == 0));
  }

  /**
   * @brief Inequality operator.
   *
   * @param lhs The left-hand side basic_string_view.
   * @param rhs The right-hand side basic_string_view.
   * @return True if the strings are not equal, otherwise false.
   */
  MGUTILITY_CNSTXPR friend bool
  operator!=(basic_string_view<Char> lhs,
             basic_string_view<Char> rhs) noexcept {
    return !(lhs == rhs);
  }

  /**
   * @brief Inequality operator.
   *
   * @param lhs The left-hand side basic_string_view.
   * @param rhs The right-hand side C-string.
   * @return True if the strings are not equal, otherwise false.
   */
  MGUTILITY_CNSTXPR friend bool operator!=(basic_string_view<Char> lhs,
                                           const Char *rhs) noexcept {
    return !(lhs == rhs);
  }

  /**
   * @brief Converts the string view to an std::string.
   *
   * @return An std::string representing the same string.
   */
  // NOLINTNEXTLINE [google-explicit-constructor]
  operator std::string() { return std::string(data_, size_); }

  /**
   * @brief Converts the string view to an std::string (const version).
   *
   * @return An std::string representing the same string.
   */
  // NOLINTNEXTLINE [google-explicit-constructor]
  operator std::string() const { return std::string(data_, size_); }

  /**
   * @brief Stream insertion operator.
   *
   * @param os The output stream.
   * @param sv The basic_string_view.
   * @return A reference to the output stream.
   */
  // NOLINTNEXTLINE [readability-identifier-length]
  friend std::ostream &
  operator<<(std::ostream &os,
             // NOLINTNEXTLINE [readability-identifier-length]
             const basic_string_view<Char> &sv) {
    // NOLINTNEXTLINE [readability-identifier-length]
    for (auto c : sv) {
      os << c;
    }
    return os;
  }

  static constexpr auto npos = -1;

private:
  size_t size_;
  const Char *data_;
};

using string_view = basic_string_view<char>;

#else

using string_view = std::string_view;

#endif

} // namespace mgutility

#endif // STRING_STRING_VIEW_HPP

#ifndef MGUTILITY_FIXED_STRING_HPP
#define MGUTILITY_FIXED_STRING_HPP

namespace mgutility {

template <size_t N = 0> class fixed_string {
public:
  template <size_t M>
  // NOLINTNEXTLINE [cppcoreguidelines-avoid-c-arrays]
  MGUTILITY_CNSTXPR static auto make(const char (&str)[M]) -> fixed_string<M> {
    return fixed_string<M>{str};
  }

  MGUTILITY_CNSTXPR fixed_string() = default;

  // Constructor to initialize from a string literal
  // NOLINTNEXTLINE [cppcoreguidelines-avoid-c-arrays]
  MGUTILITY_CNSTXPR explicit fixed_string(const char (&str)[N]) {
    for (size_t i = 0; i < N - 1; ++i) {
      data[i] = str[i];
    }
    cursor = N - 1;
    data[cursor] = '\0';
  }

  // Concatenation operator
  template <size_t M>
  MGUTILITY_CNSTXPR auto operator+(const fixed_string<M> &other) const
      -> fixed_string<N + M - 1> {
    fixed_string<N + M - 1> result{};
    size_t idx = 0;
    for (; idx < N - 1; ++idx) {
      // NOLINTNEXTLINE [cppcoreguidelines-pro-bounds-constant-array-index]
      result.data[idx] = data[idx];
    }
    for (size_t j = 0; j < M; ++j) {
      // NOLINTNEXTLINE [cppcoreguidelines-pro-bounds-constant-array-index]
      result.data[idx + j] = other.data[j];
    }
    result.cursor = N + M - 2;
    // NOLINTNEXTLINE [cppcoreguidelines-pro-bounds-constant-array-index]
    result.data[result.cursor] = '\0';
    return result;
  }

  // Concatenation operator
  template <size_t M>
  // NOLINTNEXTLINE [cppcoreguidelines-avoid-c-arrays]
  MGUTILITY_CNSTXPR auto operator+(const char (&str)[M]) const
      -> fixed_string<N + M - 1> {
    return *this + fixed_string<M>{str};
  }

  template <size_t M>
  // NOLINTNEXTLINE [cppcoreguidelines-avoid-c-arrays]
  MGUTILITY_CNSTXPR auto append(const char (&str)[M]) -> fixed_string<N> & {
    static_assert(N > M,
                  "Capacity needs to be greater than string to be appended!");
    for (size_t i = 0; i < M - 1; ++i) {
      // NOLINTNEXTLINE [cppcoreguidelines-pro-bounds-constant-array-index]
      data[cursor++] = str[i];
    }
    // NOLINTNEXTLINE [cppcoreguidelines-pro-bounds-constant-array-index]
    data[cursor] = '\0';
    return *this;
  }

  MGUTILITY_CNSTXPR auto append(string_view str) -> fixed_string<N> & {
    for (const char chr : str) {
      // NOLINTNEXTLINE [cppcoreguidelines-pro-bounds-constant-array-index]
      data[cursor++] = chr;
    }
    // NOLINTNEXTLINE [cppcoreguidelines-pro-bounds-constant-array-index]
    data[cursor] = '\0';
    return *this;
  }

  MGUTILITY_CNSTXPR auto pop_back() -> void {
    if (cursor > 0) {
      // NOLINTNEXTLINE [cppcoreguidelines-pro-bounds-constant-array-index]
      data[--cursor] = '\0';
    }
  }

  MGUTILITY_CNSTXPR auto size() const -> size_t { return cursor; }

  // NOLINTNEXTLINE [readability-identifier-length]
  constexpr size_t find(char c, size_t pos = 0) const noexcept {
    // NOLINTNEXTLINE [readability-avoid-nested-conditional-operator]
    return c == data[pos] ? pos : (pos < cursor ? find(c, ++pos) : npos);
  }

  // Conversion to std::string_view for easy printing
  // NOLINTNEXTLINE[google-explicit-constructor]
  MGUTILITY_CNSTXPR operator string_view() const {
    return string_view(data, cursor);
  }

  MGUTILITY_CNSTXPR auto view() const -> string_view {
    return string_view(data, cursor);
  }

  constexpr bool empty() const noexcept { return cursor == 0; }

  constexpr const char &operator[](size_t index) const noexcept {
    // NOLINTNEXTLINE [cppcoreguidelines-pro-bounds-constant-array-index]
    return data[index];
  }

  MGUTILITY_CNSTXPR inline bool operator==(const char *rhs) const {
    return view() == rhs;
  }

  // NOLINTNEXTLINE [readability-identifier-length]
  friend std::ostream &operator<<(std::ostream &os,
                                  const fixed_string<N> &str) {
    for (size_t i = 0; i < str.cursor; ++i) {
      // NOLINTNEXTLINE [cppcoreguidelines-pro-bounds-constant-array-index]
      os << str.data[i];
    }
    return os;
  }

  static constexpr auto npos = -1;

private:
  // NOLINTNEXTLINE [cppcoreguidelines-avoid-c-arrays]
  char data[N]{'\0'};
  size_t cursor{};
};

} // namespace mgutility

#if defined(__cpp_lib_format)

#include <format>

/**
 * @brief Formatter for enum types for use with std::format.
 *
 * @tparam Enum The enum type.
 */
template <size_t N>
struct std::formatter<mgutility::fixed_string<N>>
    : formatter<std::string_view> {
  auto constexpr format(const mgutility::fixed_string<N> &str,
                        format_context &ctx) const {
    return formatter<std::string_view>::format(str.view().data(), ctx);
  }
};

#endif

#if defined(ENUM_NAME_USE_FMT) ||                                              \
    (defined(MGUTILITY_HAS_HAS_INCLUDE) && __has_include(<fmt/format.h>))
#include <fmt/format.h>

template <size_t N>
struct fmt::formatter<mgutility::fixed_string<N>> : formatter<string_view> {
  auto format(const mgutility::fixed_string<N> &str, format_context &ctx) const
      -> appender {
    return formatter<string_view>::format(str.view().data(), ctx);
  }
};
#endif // MGUTILITY_USE_FMT || __has_include(<fmt/format.h>)

#endif // MGUTILITY_FIXED_STRING_HPP

#ifndef DETAIL_OPTIONAL_HPP
#define DETAIL_OPTIONAL_HPP

// NOLINTNEXTLINE [unused-includes]
#include <exception>

#if MGUTILITY_CPLUSPLUS >= 201703L
#include <optional>
#endif

namespace mgutility {

#if MGUTILITY_CPLUSPLUS < 201703L

/**
 * @brief Exception thrown when accessing an empty optional.
 */
struct bad_optional_access : public std::exception {
  /**
   * @brief Returns the exception message.
   *
   * @return A C-string with the exception message.
   */
  const char *what() const noexcept override { return "optional has no value"; }
};

struct nullopt_t;

/**
 * @brief A class template that provides optional (nullable) objects.
 *
 * @tparam T The type of the value.
 */
template <typename T> class optional {
public:
  /**
   * @brief Constructs an empty optional.
   */
  MGUTILITY_CNSTXPR inline explicit optional(nullopt_t & /*unused*/)
      : dummy_{0}, has_value_{false} {}

  /**
   * @brief Constructs an empty optional.
   */
  MGUTILITY_CNSTXPR inline optional() : dummy_{0}, has_value_{false} {}

  /**
   * @brief Constructs an optional with a value.
   *
   * @tparam Args The types of the arguments.
   * @param args The arguments to construct the value.
   */
  template <typename... Args>
  MGUTILITY_CNSTXPR inline explicit optional(Args &&...args)
      : value_{T{std::forward<Args>(args)...}}, has_value_{true} {}

  /**
   * @brief Constructs an optional with a value.
   *
   * @param value The value to initialize with.
   */
  MGUTILITY_CNSTXPR inline explicit optional(T &&value)
      : value_{std::move(value)}, has_value_{true} {}

  /**
   * @brief Copy constructor.
   *
   * @param other The other optional to copy.
   */
  MGUTILITY_CNSTXPR inline optional(const optional &other)
      : value_{other.value_}, has_value_{other.has_value_} {}

  /**
   * @brief Move constructor.
   *
   * @param other The other optional to move.
   */
  MGUTILITY_CNSTXPR inline optional(optional &&other) noexcept
      : value_{other.value_}, has_value_{other.has_value_} {
    other.reset();
  }

  /**
   * @brief Destructor.
   */
  ~optional() = default;

  /**
   * @brief Copy assignment operator.
   *
   * @param other The other optional to copy.
   * @return A reference to this optional.
   */
  MGUTILITY_CNSTXPR inline optional &operator=(const optional &other) {
    value_ = other.value_;
    has_value_ = other.has_value_;
    return *this;
  }

  /**
   * @brief Move assignment operator.
   *
   * @param other The other optional to move.
   * @return A reference to this optional.
   */
  MGUTILITY_CNSTXPR inline optional &operator=(optional &&other) noexcept {
    value_ = other.value_;
    has_value_ = other.has_value_;
    other.reset();
    return *this;
  }

  /**
   * @brief Swaps the contents of this optional with another.
   *
   * @param other The other optional to swap with.
   */
  MGUTILITY_CNSTXPR inline void swap(optional &other) noexcept {
    auto val = std::move(other.value_);
    other.value_ = std::move(value_);
    value_ = std::move(val);

    auto hval = other.has_value_;
    other.has_value_ = has_value_;
    has_value_ = hval;
  }

  /**
   * @brief Dereferences the stored value.
   *
   * @return A reference to the stored value.
   */
  MGUTILITY_CNSTXPR inline T &operator*() { return value_; }

  /**
   * @brief Dereferences the stored value (const version).
   *
   * @return A reference to the stored value.
   */
  MGUTILITY_CNSTXPR inline T &operator*() const { return value_; }

  /**
   * @brief Accesses the stored value.
   *
   * @return A reference to the stored value.
   * @throws bad_optional_access if the optional has no value.
   */
  MGUTILITY_CNSTXPR inline T &value() {
    if (!has_value_) {
      throw bad_optional_access();
    }
    return value_;
  }

  /**
   * @brief Accesses the stored value (const version).
   *
   * @return A reference to the stored value.
   * @throws bad_optional_access if the optional has no value.
   */
  MGUTILITY_CNSTXPR inline T &value() const {
    if (!has_value_) {
      throw bad_optional_access();
    }
    return value_;
  }

  /**
   * @brief Returns the stored value or a default value if empty.
   *
   * @param value The default value to return if empty.
   * @return The stored value or the default value.
   */
  MGUTILITY_CNSTXPR inline T value_or(T &&value) {
    return has_value_ ? value_ : std::move(value);
  }

  /**
   * @brief Returns the stored value or a default value if empty (const
   * version).
   *
   * @param value The default value to return if empty.
   * @return The stored value or the default value.
   */
  MGUTILITY_CNSTXPR inline T value_or(T &&value) const {
    return has_value_ ? value_ : std::move(value);
  }

  /**
   * @brief Returns the stored value or a default value if empty.
   *
   * @param value The default value to return if empty.
   * @return The stored value or the default value.
   */
  MGUTILITY_CNSTXPR inline T value_or(const T &value) {
    return has_value_ ? value_ : value;
  }

  /**
   * @brief Returns the stored value or a default value if empty (const
   * version).
   *
   * @param value The default value to return if empty.
   * @return The stored value or the default value.
   */
  MGUTILITY_CNSTXPR inline T value_or(const T &value) const {
    return has_value_ ? value_ : value;
  }

  /**
   * @brief Constructs the value in-place.
   *
   * @param value The value to emplace.
   */
  MGUTILITY_CNSTXPR inline void emplace(T value) {
    value_ = std::move(value);
    has_value_ = true;
  }

  /**
   * @brief Constructs the value in-place with arguments.
   *
   * @tparam Args The types of the arguments.
   * @param args The arguments to construct the value.
   */
  template <typename... Args>
  MGUTILITY_CNSTXPR inline void emplace(Args &&...args) {
    value_ = std::move(T{std::forward<Args>(args)...});
    has_value_ = true;
  }

  /**
   * @brief Checks if the optional has a value.
   *
   * @return True if the optional has a value, otherwise false.
   */
  MGUTILITY_CNSTXPR inline bool has_value() const { return has_value_; }

  /**
   * @brief Resets the optional, making it empty.
   */
  template <typename U = T,
            detail::enable_if_t<!std::is_destructible<U>::value, bool> = true>
  MGUTILITY_CNSTXPR inline void reset() {
    value_ = T{};
    has_value_ = false;
  }

  /**
   * @brief Resets the optional, making it empty.
   */
  template <typename U = T,
            detail::enable_if_t<std::is_destructible<T>::value, bool> = true>
  MGUTILITY_CNSTXPR inline void reset() {
    value_.~T();
    has_value_ = false;
  }

  /**
   * @brief Checks if the optional has a value.
   *
   * @return True if the optional has a value, otherwise false.
   */
  MGUTILITY_CNSTXPR operator bool() { return has_value_; }

private:
  union {
    T value_;
    char dummy_[sizeof(T)];
  };
  bool has_value_;
};

/**
 * @brief Represents a null optional.
 */
struct nullopt_t {
  /**
   * @brief Converts nullopt_t to an empty optional.
   *
   * @tparam T The type of the optional.
   * @return An empty optional.
   */
  template <typename T> MGUTILITY_CNSTXPR operator optional<T>() {
    return optional<T>{};
  }
};

/**
 * @brief A global instance of nullopt_t to represent null optional.
 */
auto nullopt = nullopt_t{};

#else
/**
 * @brief Alias template for std::optional.
 *
 * @tparam T The type of the value.
 */
template <typename T> using optional = std::optional<T>;

/**
 * @brief A global constant representing a null optional.
 */
inline constexpr auto nullopt{std::nullopt};

#endif
} // namespace mgutility

#endif // DETAIL_OPTIONAL_HPP

#ifndef DETAIL_ENUM_FOR_EACH_HPP
#define DETAIL_ENUM_FOR_EACH_HPP

// NOLINTNEXTLINE [unused-includes]
#include <cstdint>
#include <utility>

namespace mgutility {
namespace detail {
/**
 * @brief Alias template for a string or string view type based on the presence
 * of a bitwise OR operator.
 *
 * If the type T supports the bitwise OR operator, the alias is a std::string.
 * Otherwise, it is a mgutility::string_view.
 *
 * @tparam T The type to check.
 */
template <typename T>
// NOLINTNEXTLINE [modernize-type-traits]
using string_or_view_t = typename std::conditional<
    has_bit_or<T>::value, mgutility::fixed_string<enum_name_buffer<T>::size>,
    mgutility::string_view>::type;

/**
 * @brief A pair consisting of an enum value and its corresponding string or
 * string view.
 *
 * @tparam Enum The enum type.
 */
template <typename Enum>
using enum_pair = std::pair<Enum, detail::string_or_view_t<Enum>>;
} // namespace detail

/**
 * @brief A class template for iterating over enum values.
 *
 * @tparam Enum The enum type.
 */
template <typename Enum> class enum_for_each {
  using value_type = const detail::enum_pair<Enum>;
  using size_type = std::size_t;

  /**
   * @brief An iterator for enum values.
   */
  struct enum_iter {
    using const_iter_type = int;
    using iter_type = detail::remove_const_t<const_iter_type>;
    using iterator_category = std::forward_iterator_tag;
    using value_type = const detail::enum_pair<Enum>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type *;
    using reference = value_type &;

    /**
     * @brief Default constructor initializing the iterator to the default
     * position.
     */
    enum_iter() : m_pos{} {}

    /**
     * @brief Constructor initializing the iterator to a specific position.
     *
     * @param value The initial position of the iterator.
     */
    explicit enum_iter(iter_type value) : m_pos{value} {}

    /**
     * @brief Pre-increment operator.
     *
     * @return A reference to the incremented iterator.
     */
    auto operator++() -> enum_iter & {
      ++m_pos;
      return *this;
    }

    /**
     * @brief Post-increment operator.
     *
     * @return A copy of the iterator before incrementing.
     */
    auto operator++(int) -> enum_iter {
      m_pos++;
      return *this;
    }

    /**
     * @brief Inequality comparison operator.
     *
     * @param other The other iterator to compare with.
     * @return True if the iterators are not equal, otherwise false.
     */
    auto operator!=(const enum_iter &other) const -> bool {
      return m_pos != other.m_pos;
    }

    /**
     * @brief Equality comparison operator.
     *
     * @param other The other iterator to compare with.
     * @return True if the iterators are equal, otherwise false.
     */
    auto operator==(const enum_iter &other) const -> bool {
      return m_pos == other.m_pos;
    }

    /**
     * @brief Dereference operator.
     *
     * @return The current enum pair.
     */
    auto operator*() const -> value_type;

  private:
    iter_type m_pos; /**< The current position of the iterator. */
  };

public:
  /**
   * @brief Default constructor.
   */
  enum_for_each() = default;

  /**
   * @brief Returns an iterator to the beginning of the enum range.
   *
   * @return A reference to the beginning iterator.
   */
  auto begin() -> enum_iter & { return m_begin; }

  /**
   * @brief Returns an iterator to the end of the enum range.
   *
   * @return A reference to the end iterator.
   */
  auto end() -> enum_iter & { return m_end; }

  /**
   * @brief Returns the size of the enum range.
   *
   * @return The size of the enum range.
   */
  auto size() -> std::size_t {
    return static_cast<int>(enum_range<Enum>::max) -
           static_cast<int>(enum_range<Enum>::min) + 1;
  }

private:
  enum_iter m_begin{
      static_cast<int>(enum_range<Enum>::min)}; /**< The beginning iterator. */
  enum_iter m_end{static_cast<int>(enum_range<Enum>::max) +
                  1}; /**< The end iterator. */
};
} // namespace mgutility

#endif // DETAIL_ENUM_FOR_EACH_HPP

#ifndef MGUTILITY_REFLECTION_DETAIL_ENUM_NAME_IMPL_HPP
#define MGUTILITY_REFLECTION_DETAIL_ENUM_NAME_IMPL_HPP

// NOLINTNEXTLINE [unused-includes]
#include <algorithm>
#include <array>

/**
 * @brief Checks for MSVC compiler version.
 *
 * If the MSVC version is less than 2017, an error is raised.
 */
#if defined(_MSC_VER) && _MSC_VER < 1910
#error "Requires MSVC 2017 or newer!"
/**
 * @brief Checks for Clang compiler version.
 *
 * If the Clang version is less than 6, an error is raised.
 */
#elif defined(__clang__) && __clang_major__ < 6
#error "Requires clang 6 or newer!"
/**
 * @brief Checks for GCC compiler version.
 *
 * If the GCC version is less than 9 and it is not Clang, an error is raised.
 */
#elif defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 9
#error "Requires gcc 9 or newer!"
/**
 * @brief Checks for unsupported compilers.
 *
 * If the compiler is not MSVC, Clang, or GCC, an error is raised.
 */
#elif !defined(_MSC_VER) && !defined(__clang__) && !defined(__GNUC__)
#error "Your compiler is not supported!"
#endif

/**
 * @brief Defines the MGUTILITY_CPLUSPLUS macro for MSVC and other compilers.
 *
 * For MSVC, it uses _MSVC_LANG. For other compilers, it uses __cplusplus.
 */
#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

// NOLINTNEXTLINE [modernize-concat-nested-namespaces]
namespace mgutility {
namespace detail {

#ifndef MGUTILITY_STRLEN
// NOLINTNEXTLINE [cppcoreguidelines-macro-usage]
#define MGUTILITY_STRLEN(x) sizeof(x) - 1
#endif

/**
 * @brief Provides functionality to extract and parse enum names at
 * compile-time.
 */
struct enum_type {
private:
  /**
   * @brief Extracts raw name from compiler's __PRETTY_FUNCTION__.
   *
   * @tparam Enum The enum type.
   * @tparam e The enum value.
   * @return The raw string_view from __PRETTY_FUNCTION__.
   */
  template <typename Enum, Enum e>
  MGUTILITY_CNSTXPR static mgutility::string_view raw_name() noexcept {
#if defined(__GNUC__) && !defined(__clang__) && MGUTILITY_CPLUSPLUS >= 201402L
#define PREFIX                                                                 \
  MGUTILITY_STRLEN("static constexpr mgutility::string_view "                  \
                   "mgutility::detail::enum_type::raw_name()")
#elif defined(__clang__) || defined(__GNUC__)
#define PREFIX                                                                 \
  MGUTILITY_STRLEN("static mgutility::string_view "                            \
                   "mgutility::detail::enum_type::raw_name()")
#elif defined(_MSC_VER)
#if MGUTILITY_CPLUSPLUS > 201402L
#define PREFIX                                                                 \
  MGUTILITY_STRLEN(                                                            \
      "class std::basic_string_view<char,struct std::char_traits<char> > "     \
      "__cdecl mgutility::detail::enum_type::raw_name")
#else
#define PREFIX                                                                 \
  MGUTILITY_STRLEN("class mgutility::basic_string_view<char> __cdecl "         \
                   "mgutility::detail::enum_type::raw_name")
#endif
#else
#define PREFIX 0
#endif

    return mgutility::string_view(__PRETTY_FUNCTION__ + PREFIX,
                                  MGUTILITY_STRLEN(__PRETTY_FUNCTION__) -
                                      PREFIX + 1);
  }

  /**
   * @brief Parses the enum name from the raw string based on compiler.
   *
   * @param str The raw string from __PRETTY_FUNCTION__.
   * @return The parsed enum name.
   */
  MGUTILITY_CNSTXPR static mgutility::string_view
  parse(mgutility::string_view str) noexcept {
#if defined(__clang__) || defined(__GNUC__)
#if defined(__clang__)
    auto end = str.rfind(']');
#elif defined(__GNUC__) && !defined(__clang__)
    auto end = str.rfind(';');
#endif
    // Typical form:
    // "Enum = MyEnum::Value]"

    auto pos = str.rfind('=', end);
    if (pos == mgutility::string_view::npos) {
      return {};
    }
    pos += 2; // skip "::"

    auto result = str.substr(pos, end - pos);

#elif defined(_MSC_VER)
    // MSVC: different format
    auto pos = str.rfind(',');
    if (pos == mgutility::string_view::npos)
      return {};

    ++pos;

    auto end = str.rfind('>');
    auto result = str.substr(pos, end - pos);

#else
    return {};
#endif

    if (result.empty()) {
      return {};
    }

    // invalid cases look like "(Enum)123"
    if (result[0] == '(') {
      return {};
    }

    return result.substr(result.rfind(':') + 1);
  }

public:
  /**
   * @brief Gets the name of the enum value, checking custom names first.
   *
   * @tparam Enum The enum type.
   * @tparam e The enum value.
   * @return The name of the enum value.
   */
  template <typename Enum, Enum e>
  MGUTILITY_CNSTXPR static mgutility::string_view name() noexcept {
    return parse(raw_name<Enum, e>());
  }
};

/**
 * @brief Caches an array of enum names for a given enum sequence.
 *
 * @tparam Enum The enum type.
 * @tparam Seq The enum sequence.
 */
template <typename Enum, typename Seq> struct enum_array_cache;

/**
 * @brief Specialization of enum_array_cache for enum_sequence.
 *
 * @tparam Enum The enum type.
 * @tparam Is The enum values.
 */
template <typename Enum, Enum... Is>
struct enum_array_cache<Enum, detail::enum_sequence<Enum, Is...>> {
#if MGUTILITY_CPLUSPLUS >= 201402L
  // C++17+: fully constexpr
  // NOLINTNEXTLINE [readability-redundant-inline-specifier]
  static inline constexpr std::array<mgutility::string_view, sizeof...(Is) + 1>
  value() {
    std::array<mgutility::string_view, sizeof...(Is) + 1> arr{
        "", enum_type::template name<Enum, Is>()...};

    constexpr auto map = mgutility::custom_enum<Enum>::map;

    for (const auto &pair : map) {
      const int raw = static_cast<int>(pair.first);
      const auto idx =
          static_cast<size_t>(raw - mgutility::enum_range<Enum>::min + 1);

      if (idx >= 1 && idx < arr.size()) {
        arr[idx] = pair.second;
      }
    }

    return arr;
  }
#else
  // C++11: lazy runtime array
  static const std::array<mgutility::string_view, sizeof...(Is) + 1> &value() {
    static const std::array<mgutility::string_view, sizeof...(Is) + 1> arr =
        [] {
          std::array<mgutility::string_view, sizeof...(Is) + 1> tmp{
              "", enum_type::template name<Enum, Is>()...};

          for (const auto &pair : mgutility::custom_enum<Enum>::map) {
            auto idx =
                static_cast<size_t>(static_cast<int>(pair.first) -
                                    mgutility::enum_range<Enum>::min + 1);

            if (idx >= 1 && idx < tmp.size()) {
              tmp[idx] = pair.second;
            }
          }

          return tmp;
        }();

    return arr;
  }
#endif
};

/**
 * @brief Gets an array of enum names for the given sequence.
 *
 * @tparam Enum The enum type.
 * @tparam Is The enum values.
 * @param unused The enum sequence (unused parameter).
 * @return An array of string_views containing the enum names.
 */
template <typename Enum, Enum... Is>
MGUTILITY_CNSTXPR auto
get_enum_array(detail::enum_sequence<Enum, Is...> /*unused*/) noexcept
#if MGUTILITY_CPLUSPLUS >= 201402L
    -> std::array<mgutility::string_view, sizeof...(Is) + 1> {
  return enum_array_cache<Enum, detail::enum_sequence<Enum, Is...>>::value();
#else
    -> const std::array<mgutility::string_view, sizeof...(Is) + 1> & {
  return enum_array_cache<Enum, detail::enum_sequence<Enum, Is...>>::value();
#endif
}

/**
 * @brief Gets an array of enum names for the enum type within the specified
 * range.
 *
 * @tparam Enum The enum type.
 * @tparam Min The minimum enum value.
 * @tparam Max The maximum enum value.
 * @return An array of string_views containing the enum names.
 */
template <typename Enum, int Min = mgutility::enum_range<Enum>::min,
          int Max = mgutility::enum_range<Enum>::max>
MGUTILITY_CNSTXPR auto get_enum_array() noexcept
#if MGUTILITY_CPLUSPLUS >= 201402L
    -> std::array<mgutility::string_view, Max - Min + 2> {
  return get_enum_array<Enum>(detail::make_enum_sequence<Enum, Min, Max>());
#else
    -> const std::array<mgutility::string_view, Max - Min + 2> & {
  return get_enum_array<Enum>(detail::make_enum_sequence<Enum, Min, Max>());
#endif
}

/**
 * @brief Converts a string to an enum value.
 *
 * @tparam Enum The enum type.
 * @tparam Min The minimum enum value.
 * @tparam Max The maximum enum value.
 * @param str The string view representing the enum name.
 * @return An optional enum value.
 */
template <typename Enum, int Min, int Max>
MGUTILITY_CNSTXPR inline auto to_enum_impl(mgutility::string_view str) noexcept
    -> mgutility::optional<Enum> {
  MGUTILITY_CNSTXPR_CLANG_WA auto arr = get_enum_array<Enum, Min, Max>();

  const auto index{detail::find(arr, str)};
  return index == 0
             ? mgutility::nullopt
             : mgutility::optional<Enum>{static_cast<Enum>(index + Min - 1)};
}

/**
 * @brief Converts a string to an enum bitmask value.
 *
 * @tparam Enum The enum type.
 * @tparam Min The minimum enum value.
 * @tparam Max The maximum enum value.
 * @param str The string view representing the enum name.
 * @return An optional enum bitmask value.
 */
template <typename Enum, int Min, int Max>
MGUTILITY_CNSTXPR auto to_enum_bitmask_impl(mgutility::string_view str) noexcept
    -> mgutility::optional<Enum> {

  // Check if the string contains a '|' character
  if (str.find('|') == mgutility::string_view::npos) {
    return to_enum_impl<Enum, Min, Max>(str);
  }

  mgutility::optional<Enum> result{mgutility::nullopt};
  std::size_t index = 0;

  for (std::size_t i = 0; i < str.size(); ++i) {
    if (str[i] == '|') {
      auto name = str.substr(index, i - index);
      auto maybe_enum = to_enum_impl<Enum, Min, Max>(name);

      if (!name.empty() && maybe_enum) {
        result.emplace(result ? static_cast<Enum>(*result | *maybe_enum)
                              : *maybe_enum);
      }

      index = i + 1;
    }
  }

  auto maybe_enum = to_enum_impl<Enum, Min, Max>(str.substr(index));
  if (result && maybe_enum) {
    result.emplace(static_cast<Enum>(*result | *maybe_enum));
  } else {
    result.reset();
  }

  return result;
}

/**
 * @brief Gets the name of an enum value.
 *
 * @tparam Enum The enum type.
 * @tparam Min The minimum enum value.
 * @tparam Max The maximum enum value.
 * @param e The enum value.
 * @return A string view representing the name of the enum value.
 */
template <typename Enum, int Min, int Max,
          detail::enable_if_t<!detail::has_bit_or<Enum>::value, bool> = true>
MGUTILITY_CNSTXPR auto enum_name_impl(Enum enumValue) noexcept
    -> mgutility::string_view {
  MGUTILITY_CNSTXPR_CLANG_WA auto arr = get_enum_array<Enum, Min, Max>();
  const auto index{(Min < 0 ? -Min : Min) + static_cast<int>(enumValue) + 1};
  return arr[static_cast<size_t>(
      (index < Min || index > static_cast<int>(arr.size()) - 1) ? 0 : index)];
}

/**
 * @brief Gets the name of an enum bitmask value.
 *
 * @tparam Enum The enum type.
 * @tparam Min The minimum enum value.
 * @tparam Max The maximum enum value.
 * @param e The enum value.
 * @return A string view or string representing the name of the enum bitmask
 * value.
 */
// NOLINTNEXTLINE [modernize-use-constraints]
template <typename Enum, int Min, int Max,
          detail::enable_if_t<detail::has_bit_or<Enum>::value, bool> = true>
MGUTILITY_CNSTXPR_CLANG_WA auto enum_name_impl(Enum enumValue) noexcept
    -> mgutility::fixed_string<enum_name_buffer<Enum>::size> {

  // Get the array of enum names
  MGUTILITY_CNSTXPR_CLANG_WA auto arr = get_enum_array<Enum, Min, Max>();

  // Calculate the index in the array
  const auto index = (Min < 0 ? -Min : Min) + static_cast<int>(enumValue) + 1;
  const auto name =
      arr[(index < Min || index >= static_cast<int>(arr.size())) ? 0 : index];

  // Return the name if it's valid
  if (!name.empty() && !is_digit(name[0])) {
    return mgutility::fixed_string<enum_name_buffer<Enum>::size>{}.append(name);
  }

  // Construct bitmasked name
  mgutility::fixed_string<enum_name_buffer<Enum>::size> bitmasked_name;
  for (auto i = Min; i < Max; ++i) {
    const auto idx = (Min < 0 ? -Min : Min) + i + 1;
    if (idx >= 0 && idx < static_cast<int>(arr.size()) && !arr[idx].empty() &&
        !detail::is_digit(arr[idx][0]) &&
        (enumValue & static_cast<Enum>(i)) == static_cast<Enum>(i)) {
      bitmasked_name.append(arr[idx]).append("|");
    }
  }

  // Remove the trailing '|' if present
  if (!bitmasked_name.empty()) {
    bitmasked_name.pop_back();
  }

  return bitmasked_name;
}
} // namespace detail
} // namespace mgutility

#endif // MGUTILITY_REFLECTION_DETAIL_ENUM_NAME_IMPL_HPP

#ifndef MGUTILITY_ENUM_NAME_HPP
#define MGUTILITY_ENUM_NAME_HPP

namespace mgutility {

/**
 * @brief Converts an enum value to its underlying integer value.
 *
 * @tparam Enum The enum type.
 * @param e The enum value.
 * @return The underlying integer value of the enum.
 */
template <typename Enum,
          // NOLINTNEXTLINE [modernize-type-traits]
          detail::enable_if_t<std::is_enum<Enum>::value, bool> = true>
constexpr auto to_underlying(Enum enumValue) noexcept
    -> detail::underlying_type_t<Enum> {
  // NOLINTNEXTLINE [modernize-type-traits]
  static_assert(std::is_enum<Enum>::value, "Value is not an Enum type!");
  return static_cast<detail::underlying_type_t<Enum>>(enumValue);
}

/**
 * @brief Gets the name of an enum value.
 *
 * @tparam Min The minimum enum value.
 * @tparam Max The maximum enum value.
 * @tparam Enum The enum type.
 * @param e The enum value.
 * @return A string view or string representing the name of the enum value.
 */
template <int Min, int Max, typename Enum>
MGUTILITY_CNSTXPR auto enum_name(Enum enumValue) noexcept
    -> detail::string_or_view_t<Enum> {
  static_assert(Min < Max, "Max must be greater than Min!");
  // NOLINTNEXTLINE [modernize-type-traits]
  static_assert(std::is_enum<Enum>::value, "Value is not an Enum type!");
  return detail::enum_name_impl<Enum, Min, Max>(enumValue);
}

/**
 * @brief Gets the name of an enum value.
 *
 * @tparam Enum The enum type.
 * @tparam Min The minimum enum value, default is enum_range<Enum>::min.
 * @tparam Max The maximum enum value, default is enum_range<Enum>::max.
 * @param e The enum value.
 * @return A string view or string representing the name of the enum value.
 */
template <typename Enum, int Min = static_cast<int>(enum_range<Enum>::min),
          int Max = static_cast<int>(enum_range<Enum>::max)>
MGUTILITY_CNSTXPR auto enum_name(Enum enumValue) noexcept
    -> detail::string_or_view_t<Enum> {
  static_assert(Min < Max, "Max must be greater than Min!");
  // NOLINTNEXTLINE [modernize-type-traits]
  static_assert(std::is_enum<Enum>::value, "Value is not an Enum type!");
  return detail::enum_name_impl<Enum, Min, Max>(enumValue);
}

/**
 * @brief Gets the enum value and its name.
 *
 * @tparam Enum The enum type.
 * @return A pair of the enum value and its name.
 */
template <typename Enum>
auto enum_for_each<Enum>::enum_iter::operator*() const -> value_type {
  auto value = static_cast<Enum>(m_pos);
  return detail::enum_pair<Enum>{value, enum_name(value)};
}

/**
 * @brief Converts a string to an enum value.
 *
 * @tparam Enum The enum type.
 * @tparam Min The minimum enum value, default is enum_range<Enum>::min.
 * @tparam Max The maximum enum value, default is enum_range<Enum>::max.
 * @param str The string view representing the enum name.
 * @return An optional enum value.
 */
template <typename Enum, int Min = static_cast<int>(enum_range<Enum>::min),
          int Max = static_cast<int>(enum_range<Enum>::max),
          detail::enable_if_t<!detail::has_bit_or<Enum>::value, bool> = true>
MGUTILITY_CNSTXPR auto to_enum(mgutility::string_view str) noexcept
    -> mgutility::optional<Enum> {
  static_assert(Min < Max, "Max must be greater than Min!");
  // NOLINTNEXTLINE [modernize-type-traits]
  static_assert(std::is_enum<Enum>::value, "Type is not an Enum type!");
  return detail::to_enum_impl<Enum, Min, Max>(str);
}

/**
 * @brief Converts a string to an enum bitmask value.
 *
 * @tparam Enum The enum type.
 * @tparam Min The minimum enum value, default is enum_range<Enum>::min.
 * @tparam Max The maximum enum value, default is enum_range<Enum>::max.
 * @param str The string view representing the enum name.
 * @return An optional enum bitmask value.
 */
template <typename Enum, int Min = static_cast<int>(enum_range<Enum>::min),
          int Max = static_cast<int>(enum_range<Enum>::max),
          detail::enable_if_t<detail::has_bit_or<Enum>::value, bool> = true>
MGUTILITY_CNSTXPR auto to_enum(mgutility::string_view str) noexcept
    -> mgutility::optional<Enum> {
  static_assert(Min < Max, "Max must be greater than Min!");
  // NOLINTNEXTLINE [modernize-type-traits]
  static_assert(std::is_enum<Enum>::value, "Type is not an Enum type!");
  return detail::to_enum_bitmask_impl<Enum, Min, Max>(str);
}

/**
 * @brief Casts an integer value to an enum value.
 *
 * @tparam Enum The enum type.
 * @tparam Min The minimum enum value, default is enum_range<Enum>::min.
 * @tparam Max The maximum enum value, default is enum_range<Enum>::max.
 * @param value The integer value to cast.
 * @return An optional enum value.
 */
template <typename Enum, int Min = static_cast<int>(enum_range<Enum>::min),
          int Max = static_cast<int>(enum_range<Enum>::max)>
MGUTILITY_CNSTXPR auto enum_cast(int value) noexcept
    -> mgutility::optional<Enum> {
  static_assert(Min < Max, "Max must be greater than Min!");
  // NOLINTNEXTLINE [modernize-type-traits]
  static_assert(std::is_enum<Enum>::value, "Type is not an Enum type!");
  if (enum_name(static_cast<Enum>(value)).empty()) {
    return mgutility::nullopt;
  }
  return static_cast<Enum>(value);
}

namespace operators {
template <typename Enum, mgutility::detail::enable_if_t<
                             // NOLINTNEXTLINE [modernize-type-traits]
                             std::is_enum<Enum>::value, bool> = true>
constexpr auto operator&(const Enum &lhs, const Enum &rhs) -> Enum {
  return static_cast<Enum>(mgutility::to_underlying(lhs) &
                           mgutility::to_underlying(rhs));
}

template <typename Enum, mgutility::detail::enable_if_t<
                             // NOLINTNEXTLINE [modernize-type-traits]
                             std::is_enum<Enum>::value, bool> = true>
constexpr auto operator|(const Enum &lhs, const Enum &rhs) -> Enum {
  return static_cast<Enum>(mgutility::to_underlying(lhs) |
                           mgutility::to_underlying(rhs));
}
} // namespace operators

} // namespace mgutility

/**
 * @brief Outputs the name of an enum value to an output stream.
 *
 * @tparam Enum The enum type.
 * @param os The output stream.
 * @param e The enum value.
 * @return The output stream.
 */
template <typename Enum, mgutility::detail::enable_if_t<
                             // NOLINTNEXTLINE [modernize-type-traits]
                             std::is_enum<Enum>::value, bool> = true>
auto operator<<(std::ostream &outStream, Enum enumVal) -> std::ostream & {
  // NOLINTNEXTLINE [modernize-type-traits]
  static_assert(std::is_enum<Enum>::value, "Value is not an Enum type!");
  outStream << mgutility::enum_name(enumVal);
  return outStream;
}

#if defined(__cpp_lib_format)

#include <format>

/**
 * @brief Formatter for enum types for use with std::format.
 *
 * @tparam Enum The enum type.
 */
template <typename Enum>
  requires std::is_enum_v<Enum>
struct std::formatter<Enum> : formatter<std::string_view> {
  // NOLINTNEXTLINE [readability-identifier-length]
  auto constexpr format(Enum e, format_context &ctx) const {
    return formatter<std::string_view>::format(mgutility::enum_name(e), ctx);
  }
};

#endif

#if defined(ENUM_NAME_USE_FMT) ||                                              \
    (defined(MGUTILITY_HAS_HAS_INCLUDE) && __has_include(<fmt/format.h>))
#include <fmt/format.h>

template <class Enum>
struct fmt::formatter<Enum, char,
                      // NOLINTNEXTLINE [modernize-type-traits]
                      mgutility::detail::enable_if_t<std::is_enum<Enum>::value>>
    : formatter<string_view> {
  // NOLINTNEXTLINE [readability-identifier-length]
  auto format(const Enum e, format_context &ctx) const -> appender {
    return formatter<string_view>::format(
        static_cast<mgutility::string_view>(mgutility::enum_name(e)).data(),
        ctx);
  }
};
#endif // MGUTILITY_USE_FMT || __has_include(<fmt/format.h>)

#endif // MGUTILITY_ENUM_NAME_HPP