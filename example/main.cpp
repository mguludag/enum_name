#include <iostream>

#include "mgutility/reflection/enum_name.hpp"

#if defined(__cpp_lib_print)
#include <print>
#include <ranges>
#endif

#include <fmt/format.h>

enum class Position {
  Top = 1 << 0,
  Right = 1 << 1,
  Bottom = 1 << 2,
  Left = 1 << 3
};

using mgutility::operators::operator|;
using mgutility::operators::operator&;

// Define the range for Position enum values (Option 1)
template <> struct mgutility::enum_range<Position> {
  static constexpr auto min = 0;  // Minimum value
  static constexpr auto max = 16; // Maximum value
};

// Specialize individual or all enum names
template <> struct mgutility::custom_enum<Position> {
  static constexpr mgutility::flat_map<Position> map{
      {Position::Top, "TOP"},
      {Position::Right, "RIGHT"},
      {Position::Bottom, "BOTTOM"},
      {Position::Left, "LEFT"},
      {Position::Top | Position::Right | Position::Bottom | Position::Left,
       "CENTER"}};
};

int main() {
  // Specify enum range when call enum_name function (Option 2)
  // Lambda function to get enum name
  auto enum_name = [](Position pos) {
    return mgutility::enum_name<0, 16>(pos);
  };

  auto posLeft = Position::Left;
  auto posCenter =
      mgutility::to_enum<Position>("CENTER"); // Convert string to enum

#if MGUTILITY_CPLUSPLUS > 201402L &&                                           \
    ((defined(__clang__) && __clang_major__ > 11) ||                           \
     (defined(__GNUC__) && __GNUC__ > 11))
  static_assert(mgutility::enum_name(Position::Top | Position::Right) ==
                    "TOP|RIGHT",
                "Compile-time check failed: TOP|RIGHT");
#if MGUTILITY_CPLUSPLUS > 201703L &&                                           \
    (defined(__clang__) || (defined(__GNUC__) && __GNUC__ > 11))
  static_assert(mgutility::to_enum<Position>("BOTTOM|LEFT").value() ==
                    (Position::Bottom | Position::Left),
                "Compile-time check failed");
#endif
#endif

#if defined(__cpp_lib_print)

  //  Print each Position and its underlying value using ranges
  auto positions =
      mgutility::enum_for_each<Position>() |
      std::ranges::views::filter([](auto &&pair) {
        return !pair.second.empty() &&
               pair.second.find('|') == mgutility::string_view::npos;
      });

  std::ranges::for_each(positions, [](auto &&pos) {
    std::println("{} \t: {}", mgutility::enum_to_underlying(pos.first),
                 pos.second);
  });

#else

  // Print each Position and its underlying value using a for loop
  for (auto &&elem : mgutility::enum_for_each<Position>()) {
    if (!elem.second.empty() &&
        elem.second.find('|') == mgutility::string_view::npos) {
      fmt::print("{} \t: {}\n", mgutility::enum_to_underlying(elem.first),
                 elem.second);
    }
  }
#endif

  // Print the value of x
  std::cout << '\n' << posLeft << '\n';

  // Print the name of y or "TOP" if y is not valid
  std::cout << posCenter.value_or(Position::Top) << '\n';
}