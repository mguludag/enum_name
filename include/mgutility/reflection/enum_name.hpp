/*
MIT License

Copyright (c) 2023 mguludag

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

#ifndef MGUTILITY_ENUM_NAME_HPP
#define MGUTILITY_ENUM_NAME_HPP

#include "detail/enum_name_impl.hpp"

namespace mgutility {

/**
 * @brief Converts an enum value to its underlying integer value.
 *
 * @tparam Enum The enum type.
 * @param e The enum value.
 * @return The underlying integer value of the enum.
 */
template <typename Enum>
constexpr auto enum_to_underlying(Enum enumValue) noexcept
    -> detail::underlying_type_t<Enum> {
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
  static_assert(std::is_enum<Enum>::value, "Type is not an Enum type!");
  if (enum_name(static_cast<Enum>(value)).empty()) {
    return mgutility::nullopt;
  }
  return static_cast<Enum>(value);
}

namespace operators {
template <typename Enum, mgutility::detail::enable_if_t<
                             std::is_enum<Enum>::value, bool> = true>
 constexpr auto operator&(const Enum &lhs, const Enum &rhs) -> Enum {
  return static_cast<Enum>(mgutility::enum_to_underlying(lhs) &
                           mgutility::enum_to_underlying(rhs));
}

template <typename Enum, mgutility::detail::enable_if_t<
                             std::is_enum<Enum>::value, bool> = true>
 constexpr auto operator|(const Enum &lhs, const Enum &rhs) -> Enum {
  return static_cast<Enum>(mgutility::enum_to_underlying(lhs) |
                           mgutility::enum_to_underlying(rhs));
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
                             std::is_enum<Enum>::value, bool> = true>
auto operator<<(std::ostream &outStream, Enum enumVal) -> std::ostream & {
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
  auto constexpr format(Enum e, format_context &ctx) const {
    return formatter<std::string_view>::format(mgutility::enum_name(e), ctx);
  }
};

#endif

#if defined(__has_include)
#if __has_include(<fmt/format.h>)
#include <fmt/format.h>

template<class Enum> 
struct fmt::formatter<Enum, char, mgutility::detail::enable_if_t<
                    std::is_enum<Enum>::value>> : formatter<string_view>
{
  auto format(const Enum e, format_context &ctx) const -> appender {
    return formatter<string_view>::format(static_cast<
        mgutility::string_view>(mgutility::enum_name(e)).data(), ctx);
  }
};
#endif
#endif

#endif // MGUTILITY_ENUM_NAME_HPP
