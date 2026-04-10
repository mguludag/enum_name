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

#ifndef MGUTILITY_REFLECTION_DETAIL_ENUM_NAME_IMPL_HPP
#define MGUTILITY_REFLECTION_DETAIL_ENUM_NAME_IMPL_HPP

// NOLINTNEXTLINE [unused-includes]
#include "enum_for_each.hpp"
#include "meta.hpp"
#include "mgutility/_common/definitions.hpp"
#include "mgutility/std/fixed_string.hpp"
#include "mgutility/std/optional.hpp"
#include "mgutility/std/string_view.hpp"
#include "mgutility/std/utility.hpp"

#include <algorithm>
#include <array>
#include <cstddef>

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

template <typename T, int Min, int Max> struct enum_name_parse_result {
  static constexpr auto size = std::size_t{Max - Min};
  fixed_string<enum_name_buffer<T>::size * size> blob;
  std::array<pair<std::size_t, std::size_t>, size> ranges;
};

template <typename T, int Min, int Max>
using enum_name_array =
    std::array<mgutility::string_view, static_cast<std::size_t>(Max - Min)>;

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
  template <typename Enum, Enum... e>
  MGUTILITY_CNSTXPR static mgutility::string_view
  raw_name(detail::enum_sequence<Enum, e...> /*unused*/) noexcept {
#if defined(__GNUC__) && !defined(__clang__) && MGUTILITY_CPLUSPLUS >= 201402L
#define PREFIX                                                                 \
  MGUTILITY_STRLEN("static constexpr mgutility::string_view "                  \
                   "mgutility::detail::enum_type::raw_name(mgutility::detail:" \
                   ":enum_sequence<Enum, e ...>) [with Enum = ")
#elif defined(__clang__) || defined(__GNUC__)
#define PREFIX                                                                 \
  MGUTILITY_STRLEN("static mgutility::string_view "                            \
                   "mgutility::detail::enum_type::raw_name(mgutility::detail:" \
                   ":enum_sequence<Enum, e ...>) [with Enum = ")
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
  template <typename Enum, int Min, int Max>
  MGUTILITY_CNSTXPR static enum_name_parse_result<Enum, Min, Max>
  parse() noexcept {
    MGUTILITY_CNSTXPR auto str =
        raw_name<Enum>(detail::make_enum_sequence<Enum, Min, Max>{});

#if defined(__clang__) || defined(__GNUC__)
#if defined(__clang__)
    auto end = str.rfind(']');
#elif defined(__GNUC__) && !defined(__clang__)
    auto end = str.rfind(';');
#endif

    auto enum_names = str.substr(0, end);

#elif defined(_MSC_VER)
    // MSVC: different format
    auto pos = str.find(',');
    if (pos == mgutility::string_view::npos)
      return {};

    ++pos;

    auto end = str.rfind('>');
    auto enum_names = str.substr(pos, end - pos);

#else
    return {};
#endif

    if (enum_names.empty()) {
      return {};
    }

    enum_name_parse_result<Enum, Min, Max> result{};

    std::size_t idx = 0;

    while (!enum_names.empty() && idx < result.ranges.size()) {
      auto pos = enum_names.find(',');
      if (pos != mgutility::string_view::npos) {
        auto token = enum_names.substr(0, pos);

        // remove whitespace
        while (!token.empty() && token.front() == ' ') {
          token = token.substr(1);
        }

        while (!token.empty() && token.back() == ' ') {
          token = token.substr(0, token.size() - 1);
        }

        std::size_t begin = token.find('(');
        std::size_t end = token.rfind(')');
        if (begin != mgutility::string_view::npos ||
            end != mgutility::string_view::npos) {
          result.ranges[idx++] = {0, 0};
          enum_names = enum_names.substr(pos + 1);
          continue;
        }

        std::size_t start = 0;
        if (token.find(':') != mgutility::string_view::npos) {
          start = token.find(':') + 2;
          token = token.substr(start);
        }

        auto size = result.blob.size();
        result.blob.append(token);
        result.ranges[idx++] = {size, result.blob.size() - size};

        enum_names = enum_names.substr(pos + 1);
        continue;
      }
      enum_names = {};
    }

    return result;
  }

public:
  /**
   * @brief Gets the name of the enum value, checking custom names first.
   *
   * @tparam Enum The enum type.
   * @tparam e The enum value.
   * @return The name of the enum value.
   */
  template <typename Enum, int Min, int Max>
  MGUTILITY_CNSTXPR static enum_name_parse_result<Enum, Min, Max>
  name() noexcept {
    return parse<Enum, Min, Max>();
  }
};

/**
 * @brief Caches an array of enum names for a given enum sequence.
 *
 * @tparam Enum The enum type.
 * @tparam Seq The enum sequence.
 */
template <typename Enum, int Min, int Max> struct enum_array_cache;

/**
 * @brief Specialization of enum_array_cache for enum_sequence.
 *
 * @tparam Enum The enum type.
 * @tparam Is The enum values.
 */
template <typename Enum, int Min, int Max> struct enum_array_cache {

#if MGUTILITY_CPLUSPLUS > 201402L

  static inline constexpr auto parse_result =
      enum_type::template name<Enum, Min, Max>();

#else
  // C++11: lazy runtime array
  static enum_name_parse_result<Enum, Min, Max> &value() {
    static enum_name_parse_result<Enum, Min, Max> arr =
        enum_type::template name<Enum, Min, Max>();

    return arr;
  }
#endif

  static MGUTILITY_CNSTXPR auto
  apply_custom(const enum_name_parse_result<Enum, Min, Max> &result) noexcept
      -> enum_name_array<Enum, Min, Max> {
    enum_name_array<Enum, Min, Max> arr{};

    for (std::size_t idx = 0; idx < result.ranges.size(); ++idx) {
      arr[idx] = result.blob.view().substr(result.ranges[idx].first,
                                           result.ranges[idx].second);
    }

#if MGUTILITY_CPLUSPLUS >= 201402L
    constexpr auto map = mgutility::custom_enum<Enum>::map;
    for (const auto &pair : map) {
#else
    for (const auto &pair : mgutility::custom_enum<Enum>::map) {
#endif
      if (pair.first >= static_cast<Enum>(Min) &&
          pair.first < static_cast<Enum>(Max)) {
        arr[static_cast<std::size_t>(pair.first) - Min] = pair.second;
      }
    }

    return arr;
  }
};

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
    -> enum_name_array<Enum, Min, Max> {
#if MGUTILITY_CPLUSPLUS > 201402L
  constexpr auto &cache = enum_array_cache<Enum, Min, Max>::parse_result;
  return enum_array_cache<Enum, Min, Max>::apply_custom(cache);
#else
  return enum_array_cache<Enum, Min, Max>::apply_custom(
      enum_array_cache<Enum, Min, Max>::value());
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
  return index == 0 ? mgutility::nullopt
                    : mgutility::optional<Enum>{static_cast<Enum>(index + Min)};
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
  const auto index{(Min < 0 ? -Min : Min) + static_cast<int>(enumValue)};
  if (index < Min || index > static_cast<int>(arr.size()) - 1) {
    return mgutility::string_view{};
  }

  return arr[static_cast<size_t>(index)];
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
  const auto index = (Min < 0 ? -Min : Min) + static_cast<int>(enumValue);

  mgutility::fixed_string<enum_name_buffer<Enum>::size> bitmasked_name;

  if (index >= 0 && index < static_cast<int>(arr.size())) {
    bitmasked_name.append(arr[static_cast<size_t>(index)]);
  }

  if (!bitmasked_name.empty()) {
    return bitmasked_name;
  }

  for (auto i = 0; i < Max - Min; ++i) {
    if (i >= 0 && i < static_cast<int>(arr.size()) && arr[i].size() > 0 &&
        (enumValue & static_cast<Enum>(i)) == static_cast<Enum>(i)) {
      bitmasked_name.append(arr[i]).append("|");
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
