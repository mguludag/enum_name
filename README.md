# enum_name

Converting (scoped)enum values to/from string names written in C++>=11.

## Supported Compilers

- Clang > 5
- GCC > 8
- MSVC > 2015

## Features

- Supports `enum` and `enum class`
- Supports enums in namespaces, classes or structs even templated or not
- Supports full compile-time with C++20 and later
- Changing enum range with template parameter <sub>(default range: `[0, 256)`)</sub> on each call or with your special function for types or adding specialized `enum_range<Enum>` struct
- Supports and automatically overloaded `operator<<` and add `std::formatter` specialization for Enum types to direct using with ostream objects and `std::format` (If `fmtlib` is available, simply adding the specialization like this [`template <> struct fmt::formatter<YourEnumType> : ostream_formatter {};`] is enough.)
- Supports custom enum name(s) input/output by explicit specialization of `template <> struct mgutility::custom_enum<Enum>` with `static constexpr mgutility::flat_map<Enum> map{{Enum, const char*},...}` variable.
- Supports bitmasked enums and auto detect them
- Supports iterate over enum (names and values) with `mgutility::enum_for_each<T>()` class and it is compatible with standard ranges and views

## Limitations

- Compiler versions
- Wider range can increase compile time so user responsible to adjusting for enum's range

## Fetch library with CMake

```CMake
include(FetchContent)

FetchContent_Declare(
  enum_name
  GIT_REPOSITORY https://github.com/mguludag/enum_name.git
  GIT_TAG main # or the specific tag or branch you want to use
)

FetchContent_MakeAvailable(enum_name)

#...

target_link_libraries(${PROJECT_NAME} PRIVATE mgutility::enum_name)
```

## Example usage ([try it!](https://godbolt.org/z/1nqrj78vb))

```C++
#include <iostream>

#include "mgutility/reflection/enum_name.hpp"

#if defined(__cpp_lib_print)
#include <print>
#include <ranges>
#endif

enum class Position {
  Top = 1 << 0,
  Right = 1 << 1,
  Bottom = 1 << 2,
  Left = 1 << 3
};

// Define bitwise OR operator for Position
auto constexpr operator|(Position lhs, Position rhs) -> Position {
  return static_cast<Position>(mgutility::enum_to_underlying(lhs) |
                               mgutility::enum_to_underlying(rhs));
}

// Define bitwise AND operator for Position
auto constexpr operator&(Position lhs, Position rhs) -> Position {
  return static_cast<Position>(mgutility::enum_to_underlying(lhs) &
                               mgutility::enum_to_underlying(rhs));
}

// Define the range for Position enum values (Option 1)
template <>
struct mgutility::enum_range<Position> {
  static constexpr auto min = 0;  // Minimum value
  static constexpr auto max = 16; // Maximum value
};

// Specialize individual or all enum names
template <>
struct mgutility::custom_enum<Position> {
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
  auto enum_name = [](Position c) { return mgutility::enum_name<0, 16>(c); };

  auto x = Position::Left;
  auto y = mgutility::to_enum<Position>("CENTER"); // Convert string to enum

#if defined(__cpp_lib_constexpr_string)
  static_assert(mgutility::enum_name(Position::Top | Position::Right) ==
                "TOP|RIGHT"); // Compile-time check
  static_assert(mgutility::to_enum<Position>("BOTTOM|LEFT") ==
                (Position::Bottom | Position::Left)); // Compile-time check
#endif

#if defined(__cpp_lib_print)

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
```
