#include <iostream>

#include "mgutility/reflection/enum_name.hpp"

#if defined(__cpp_lib_print)
#include <print>
#include <ranges>
#endif

enum class Position : std::uint8_t {
  Top = 1 << 0,
  Right = 1 << 1,
  Bottom = 1 << 2,
  Left = 1 << 3
};


// Define bitwise OR operator for Position
constexpr static auto operator|(Position lhs, Position rhs) -> Position {
  return static_cast<Position>(mgutility::enum_to_underlying(lhs) |
                               mgutility::enum_to_underlying(rhs));
}

// Define bitwise AND operator for Position
constexpr static auto operator&(Position lhs, Position rhs) -> Position {
  return static_cast<Position>(mgutility::enum_to_underlying(lhs) &
                               mgutility::enum_to_underlying(rhs));
}


// Define the range for Position enum values (Option 1)
template <> struct mgutility::enum_range<Position> {
  static constexpr auto min = 0;  // Minimum value
  static constexpr auto max = 16; // Maximum value
};

// Specialize individual or all enum names
template <> struct mgutility::custom_enum<Position> {
  static constexpr mgutility::flat_map<Position> map{
      {.first = Position::Top, .second = "TOP"},
      {.first = Position::Right, .second = "RIGHT"},
      {.first = Position::Bottom, .second = "BOTTOM"},
      {.first = Position::Left, .second = "LEFT"},
      {.first =
           Position::Top | Position::Right | Position::Bottom | Position::Left,
       .second = "CENTER"}};
};

int main() {
  int aaa = 0;

  // Specify enum range when call enum_name function (Option 2)
  // Lambda function to get enum name
  auto enum_name = [](Position c) { return mgutility::enum_name<0, 16>(c); };

  auto x = Position::Left;
  auto y = mgutility::to_enum<Position>("CENTER"); // Convert string to enum

#if defined(__cpp_lib_constexpr_string) && defined(__GNUC__) &&                \
    !defined(__clang__) && __GNUC__ > 11
  static_assert(mgutility::enum_name(Position::Top | Position::Right) ==
                "TOP|RIGHT"); // Compile-time check
  static_assert(mgutility::to_enum<Position>("BOTTOM|LEFT") ==
                (Position::Bottom | Position::Left)); // Compile-time check
#endif

#if defined(__cpp_lib_print) || __GNUC__ > 12

  // Print each Position and its underlying value using ranges
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
  for (auto &&e : mgutility::enum_for_each<Position>()) {
    if (!e.second.empty() &&
        e.second.find('|') == mgutility::string_view::npos) {
      std::cout << mgutility::enum_to_underlying(e.first) << " \t: " << e.second
                << '\n';
    }
  }
#endif

  // Print the value of x
  std::cout << '\n' << x << '\n';

  // Print the name of y or "TOP" if y is not valid
  std::cout << y.value_or(Position::Top) << '\n';
}