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

#ifndef STRING_STRING_VIEW_HPP
#define STRING_STRING_VIEW_HPP

#include <cstring>
#include <ostream>
#include <string>

#include "mgutility/_common/definitions.hpp"

namespace mgutility {

#if MGUTILITY_CPLUSPLUS < 201703L

namespace detail {
/**
 * @brief Computes the length of a C-string at compile-time.
 *
 * @param str The C-string.
 * @param sz The initial size, default is 0.
 * @return The length of the C-string.
 */
constexpr auto strlen_constexpr(const char *str, size_t sz = 0) noexcept
    -> size_t {
  return str[sz] == '\0' ? sz : strlen_constexpr(str, ++sz);
}
} // namespace detail

/**
 * @brief A basic string view class template.
 *
 * @tparam Char The character type, default is char.
 */
template <typename Char = char> class basic_string_view {
public:
  /**
   * @brief Default constructor.
   */
  constexpr inline basic_string_view() noexcept : data_(""), size_(0) {}

  /**
   * @brief Constructs a basic_string_view from a C-string.
   *
   * @param str The C-string.
   */
  constexpr inline basic_string_view(const Char *str) noexcept
      : data_(str), size_(detail::strlen_constexpr(str)) {}

  /**
   * @brief Constructs a basic_string_view from a std::string.
   *
   * @param str The std::string.
   */
  constexpr inline basic_string_view(
      const std::basic_string<Char> &str) noexcept
      : data_(str.c_str()), size_(str.size()) {}

  /**
   * @brief Constructs a basic_string_view from a C-string and length.
   *
   * @param str The C-string.
   * @param len The length of the string.
   */
  constexpr inline basic_string_view(const Char *str, size_t len) noexcept
      : data_(str), size_(len) {}

  /**
   * @brief Copy constructor.
   *
   * @param other The other basic_string_view to copy.
   */
  constexpr inline basic_string_view(const basic_string_view &other)
      : data_(other.data_), size_(other.size_) {}

  /**
   * @brief Move constructor.
   *
   * @param other The other basic_string_view to move.
   */
  constexpr inline basic_string_view(basic_string_view &&other) noexcept
      : data_(std::move(other.data_)), size_(std::move(other.size_)) {}

  /**
   * @brief Copy assignment operator.
   *
   * @param other The other basic_string_view to copy.
   * @return A reference to this object.
   */
  MGUTILITY_CNSTXPR inline basic_string_view<Char> &
  operator=(const basic_string_view &other) noexcept {
    data_ = other.data_;
    size_ = other.size_;
    return *this;
  }

  /**
   * @brief Move assignment operator.
   *
   * @param other The other basic_string_view to move.
   * @return A reference to this object.
   */
  MGUTILITY_CNSTXPR inline basic_string_view<Char> &
  operator=(basic_string_view &&other) noexcept {
    data_ = std::move(other.data_);
    size_ = std::move(other.size_);
    return *this;
  }

  /**
   * @brief Accesses the character at the given index.
   *
   * @param index The index.
   * @return The character at the index.
   */
  constexpr inline const Char operator[](size_t index) const noexcept {
    return data_[index];
  }

  /**
   * @brief Returns an iterator to the beginning of the string.
   *
   * @return A pointer to the first character.
   */
  constexpr inline const Char *begin() const noexcept { return data_; }

  /**
   * @brief Returns an iterator to the end of the string.
   *
   * @return A pointer to one past the last character.
   */
  constexpr inline const Char *end() const noexcept { return (data_ + size_); }

  /**
   * @brief Checks if the string is empty.
   *
   * @return True if the string is empty, otherwise false.
   */
  constexpr inline bool empty() const noexcept { return size_ < 1; }

  /**
   * @brief Returns the size of the string.
   *
   * @return The size of the string.
   */
  constexpr inline size_t size() const noexcept { return size_; }

  /**
   * @brief Returns a pointer to the underlying data.
   *
   * @return A pointer to the data.
   */
  constexpr inline const Char *data() const noexcept { return data_; }

  /**
   * @brief Returns a substring view.
   *
   * @param begin The starting position.
   * @param len The length of the substring.
   * @return A basic_string_view representing the substring.
   */
  constexpr inline basic_string_view<Char>
  substr(size_t begin, size_t len = 0U) const noexcept {
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
  constexpr inline size_t rfind(Char c, size_t pos = npos) const noexcept {
    return (pos == npos ? pos = size_ : pos = pos), c == data_[pos] ? pos
                                                    : pos == 0U
                                                        ? npos
                                                        : rfind(c, --pos);
  }

  /**
   * @brief Finds the first occurrence of a character.
   *
   * @param c The character to find.
   * @param pos The position to start from, default is 0.
   * @return The position of the character or npos if not found.
   */
  constexpr inline size_t find(Char c, size_t pos = 0) const noexcept {
    return c == data_[pos] ? pos : pos < size_ ? find(c, ++pos) : npos;
  }

  /**
   * @brief Equality operator.
   *
   * @param lhs The left-hand side basic_string_view.
   * @param rhs The right-hand side basic_string_view.
   * @return True if the strings are equal, otherwise false.
   */
  constexpr friend inline bool
  operator==(basic_string_view<Char> lhs,
             basic_string_view<Char> rhs) noexcept {
    return (lhs.size_ == rhs.size_) &&
           std::strncmp(lhs.data_, rhs.data_, lhs.size_) == 0;
  }

  /**
   * @brief Equality operator.
   *
   * @param lhs The left-hand side basic_string_view.
   * @param rhs The right-hand side C-string.
   * @return True if the strings are equal, otherwise false.
   */
  constexpr friend inline bool operator==(basic_string_view<Char> lhs,
                                          const Char *rhs) noexcept {
    return (lhs.size_ == detail::strlen_constexpr(rhs)) &&
           std::strncmp(lhs.data_, rhs, lhs.size_) == 0;
  }

  /**
   * @brief Inequality operator.
   *
   * @param lhs The left-hand side basic_string_view.
   * @param rhs The right-hand side basic_string_view.
   * @return True if the strings are not equal, otherwise false.
   */
  constexpr friend inline bool
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
  constexpr friend inline bool operator!=(basic_string_view<Char> lhs,
                                          const Char *rhs) noexcept {
    return !(lhs == rhs);
  }

  /**
   * @brief Converts the string view to an std::string.
   *
   * @return An std::string representing the same string.
   */
  inline operator std::string() { return std::string(data_, size_); }

  /**
   * @brief Converts the string view to an std::string (const version).
   *
   * @return An std::string representing the same string.
   */
  inline operator std::string() const { return std::string(data_, size_); }

  /**
   * @brief Stream insertion operator.
   *
   * @param os The output stream.
   * @param sv The basic_string_view.
   * @return A reference to the output stream.
   */
  friend inline std::ostream &operator<<(std::ostream &os,
                                         const basic_string_view<Char> &sv) {
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
#include <string_view>

using string_view = std::string_view;

#endif

} // namespace mgutility

#endif // STRING_STRING_VIEW_HPP