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

#include "enum_for_each.hpp"
#include "meta.hpp"
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

namespace mgutility {
namespace detail {

struct enum_type {
  /**
   * @brief Gets the name of an unscoped enum value.
   *
   * @tparam Enum The enum type.
   * @tparam e The enum value.
   * @return A string view representing the name of the enum value.
   */
  template <
      typename Enum, Enum e,
      detail::enable_if_t<!detail::is_scoped_enum<Enum>::value, bool> = true>
  MGUTILITY_CNSTXPR static auto name() noexcept -> mgutility::string_view {
    for (auto &pair : mgutility::custom_enum<Enum>::map) {
      if (pair.first == e) {
        return pair.second;
      }
    }
    MGUTILITY_CNSTXPR auto str = mgutility::string_view(__PRETTY_FUNCTION__);
    MGUTILITY_CNSTXPR auto offset = lastidxenumname[0] + lastidxenumname[1];
    MGUTILITY_CNSTXPR auto index =
        std::max(str.rfind(lastidxenumname[2], str.size() - offset),
                 str.rfind(lastidxenumname[3], str.size() - offset));
    MGUTILITY_CNSTXPR auto result =
        str.substr(index + 1, str.size() - offset - index);
    return result[0] == '(' ? "" : result;
  }

  /**
   * @brief Gets the name of a scoped enum value.
   *
   * @tparam Enum The enum type.
   * @tparam e The enum value.
   * @return A string view representing the name of the enum value.
   */
  template <
      typename Enum, Enum e,
      detail::enable_if_t<detail::is_scoped_enum<Enum>::value, bool> = true>
  MGUTILITY_CNSTXPR static auto name() noexcept -> mgutility::string_view {
    for (auto &pair : mgutility::custom_enum<Enum>::map) {
      if (pair.first == e) {
        return pair.second;
      }
    }
    MGUTILITY_CNSTXPR auto str = mgutility::string_view(__PRETTY_FUNCTION__);
    MGUTILITY_CNSTXPR auto index =
        str.rfind(lastidxenumname[3], str.size() - lastidxenumname[0]) + 1;
    MGUTILITY_CNSTXPR auto result =
        str.substr(index, str.size() - lastidxenumname[0] - index);
    MGUTILITY_CNSTXPR auto is_invalid =
            result.rfind(lastidxenumname[5]) != result.npos || (result.size() > 4 && result[4] == lastidxenumname[4]);
    return is_invalid ? "" : result;
  }

private:
  static constexpr int lastidxenumname[] =
#if defined(__clang__)
      {1, 1, ' ', ':', '(',
#if __clang_major__ < 13
         ','
#else
         ')'
#endif
    };
#elif defined(_MSC_VER)
      {21, 0, ',', ':', '<', ')'};
#elif defined(__GNUC__)
      {
#if MGUTILITY_CPLUSPLUS < 201703L
          163,
#else
          157,
#endif
          5, ' ', ':', '(', ')'};
#endif
};

/**
 * @brief Generates an array of enum names.
 *
 * @tparam Enum The enum type.
 * @tparam Is The enumeration values.
 * @return An array of string views representing the enum names.
 */
template <typename Enum, Enum... Is>
MGUTILITY_CNSTXPR inline auto
get_enum_array(detail::enum_sequence<Enum, Is...> /*unused*/) noexcept
    -> std::array<mgutility::string_view, sizeof...(Is) + 1> {
  return std::array<mgutility::string_view, sizeof...(Is) + 1>{
      "", enum_type::template name<Enum, Is>()...};
}

/**
 * @brief Generates an array of enum names.
 *
 * @tparam Enum The enum type.
 * @tparam Min The minimum enum value.
 * @tparam Max The maximum enum value.
 * @return An array of string views representing the enum names.
 */
template <typename Enum, int Min = mgutility::enum_range<Enum>::min,
          int Max = mgutility::enum_range<Enum>::max>
MGUTILITY_CNSTXPR inline auto get_enum_array() noexcept
    -> std::array<mgutility::string_view, Max - Min + 1> {
  return get_enum_array<Enum>(detail::make_enum_sequence<Enum, Min, Max>());
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
  const auto index{std::find(arr.begin() + 1, arr.end(), str)};
  return index == arr.end() ? mgutility::nullopt
                            : mgutility::optional<Enum>{static_cast<Enum>(
                                  std::distance(arr.begin(), index) + Min - 1)};
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
MGUTILITY_CNSTXPR auto enum_name_impl(Enum e) noexcept
    -> mgutility::string_view {
  MGUTILITY_CNSTXPR auto arr = get_enum_array<Enum, Min, Max>();
  const auto index{(Min < 0 ? -Min : Min) + static_cast<int>(e) + 1};
  return arr[(index < Min || index > arr.size() - 1) ? 0 : index];
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
template <typename Enum, int Min, int Max,
          detail::enable_if_t<detail::has_bit_or<Enum>::value, bool> = true>
MGUTILITY_CNSTXPR_CLANG_WA inline auto enum_name_impl(Enum e) noexcept
    -> detail::string_or_view_t<Enum> {

  // Get the array of enum names
  MGUTILITY_CNSTXPR_CLANG_WA auto arr = get_enum_array<Enum, Min, Max>();

  // Calculate the index in the array
  const auto index = (Min < 0 ? -Min : Min) + static_cast<int>(e) + 1;
  const auto name =
      arr[(index < Min || index >= static_cast<int>(arr.size())) ? 0 : index];

  // Lambda to check if a character is a digit
  const auto is_digit = [](char c) { return c >= '0' && c <= '9'; };

  // Return the name if it's valid
  if (!name.empty() && !is_digit(name[0])) {
    return std::string{name};
  }

  // Construct bitmasked name
  std::string bitmasked_name;
  for (auto i = Min; i < Max; ++i) {
    const auto idx = (Min < 0 ? -Min : Min) + i + 1;
    if (idx >= 0 && idx < static_cast<int>(arr.size()) && !arr[idx].empty() &&
        !is_digit(arr[idx][0]) &&
        (e & static_cast<Enum>(i)) == static_cast<Enum>(i)) {
      bitmasked_name.append(arr[idx]).append("|");
    }
  }

  // Remove the trailing '|' if present
  if (!bitmasked_name.empty()) {
    bitmasked_name.pop_back();
  }

  if (bitmasked_name.find('|') != std::string::npos) {
    return bitmasked_name;
  }
  return std::string{""};
}
} // namespace detail
} // namespace mgutility

#endif // MGUTILITY_REFLECTION_DETAIL_ENUM_NAME_IMPL_HPP
