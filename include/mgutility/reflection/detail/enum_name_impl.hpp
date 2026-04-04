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
#include "mgutility/std/optional.hpp"
#include "mgutility/std/string_view.hpp"

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

/**
 * @brief Provides functionality to extract and parse enum names at
 * compile-time.
 */
struct enum_type {
private:
  /**
   * @brief Looks up custom enum name for the given value.
   *
   * @tparam Enum The enum type.
   * @param e The enum value.
   * @return The custom name if found, empty string_view otherwise.
   */
  template <typename Enum>
  MGUTILITY_CNSTXPR static mgutility::string_view
  // NOLINTNEXTLINE [readability-identifier-length]
  lookup_custom(Enum e) noexcept {
    for (const auto &pair : mgutility::custom_enum<Enum>::map) {
      if (pair.first == e) {
        return pair.second;
      }
    }
    return {};
  }

  /**
   * @brief Extracts raw name from compiler's __PRETTY_FUNCTION__.
   *
   * @tparam Enum The enum type.
   * @tparam e The enum value.
   * @return The raw string_view from __PRETTY_FUNCTION__.
   */
  template <typename Enum, Enum e>
  MGUTILITY_CNSTXPR static mgutility::string_view raw_name() noexcept {
    return mgutility::string_view(__PRETTY_FUNCTION__,
                                  sizeof(__PRETTY_FUNCTION__) - 1);
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
    // 1. Custom override first
    auto custom = lookup_custom(e);
    if (!custom.empty()) {
      return custom;
    }

    // 2. Extract + parse
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
    return std::array<mgutility::string_view, sizeof...(Is) + 1>{
        "", enum_type::template name<Enum, Is>()...};
  }
#else
  // C++11: lazy runtime array
  static const std::array<mgutility::string_view, sizeof...(Is) + 1> &value() {
    static std::array<mgutility::string_view, sizeof...(Is) + 1> arr{
        "", enum_type::template name<Enum, Is>()...};
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
