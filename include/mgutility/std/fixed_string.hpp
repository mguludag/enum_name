/*
MIT License

Copyright (c) 2025 mguludag

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

#ifndef MGUTILITY_FIXED_STRING_HPP
#define MGUTILITY_FIXED_STRING_HPP

#include "mgutility/std/string_view.hpp"

namespace mgutility {

template <size_t N = 0> class fixed_string {
public:
  template <size_t M>
  MGUTILITY_CNSTXPR static auto make(const char (&str)[M]) -> fixed_string<M> {
    return fixed_string<M>{str};
  }

  MGUTILITY_CNSTXPR fixed_string() = default;

  // Constructor to initialize from a string literal
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
      result.data[idx] = data[idx];
    }
    for (size_t j = 0; j < M; ++j) {
      result.data[idx + j] = other.data[j];
    }
    result.cursor = N + M - 2;
    result.data[result.cursor] = '\0';
    return result;
  }

  // Concatenation operator
  template <size_t M>
  MGUTILITY_CNSTXPR auto operator+(const char (&str)[M]) const
      -> fixed_string<N + M - 1> {
    return *this + fixed_string<M>{str};
  }

  template <size_t M>
  MGUTILITY_CNSTXPR auto append(const char (&str)[M]) -> fixed_string<N> & {
    static_assert(N > M,
                  "Capacity needs to be greater than string to be appended!");
    for (size_t i = 0; i < M - 1; ++i) {
      data[cursor++] = str[i];
    }
    data[cursor] = '\0';
    return *this;
  }

  MGUTILITY_CNSTXPR auto append(string_view str) -> fixed_string<N> & {
    for (char chr : str) {
      data[cursor++] = chr;
    }
    data[cursor] = '\0';
    return *this;
  }

  MGUTILITY_CNSTXPR auto pop_back() -> void {
    if (cursor > 0) {
      data[--cursor] = '\0';
    }
  }

  MGUTILITY_CNSTXPR auto size() const -> size_t { return cursor; }

  constexpr size_t find(char c, size_t pos = 0) const noexcept {
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
    return data[index];
  }

  MGUTILITY_CNSTXPR inline bool operator==(const char *rhs) const {
    return string_view(*this) == rhs;
  }

  friend std::ostream &operator<<(std::ostream &os,
                                  const fixed_string<N> &str) {
    for (size_t i = 0; i < str.cursor; ++i) {
      os << str.data[i];
    }
    return os;
  }

  static constexpr auto npos = -1;

private:
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