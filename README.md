# enum_name
Converting (scoped)enum values to/from string names written in C++>=11.

## Supported Compilers
* Clang > 5
* GCC > 8
* MSVC > 2015

## Features
* Supports `enum` and `enum class`
* Supports enums in namespaces, classes or structs even templated or not
* Supports compile-time as much as possible using with C++14 and later
* Changing enum range with template parameter <sub>(default range: `[-128, 128)`)</sub> on each call or with your special function for types or adding specialized `enum_range<Enum>` struct
* Supports `operator<<` for direct using with ostream objects
* Supports custom enum name output by explicit specialization of `constexpr inline auto mgutility::detail::enum_type::name<Enum, EnumValue>() noexcept` function
* Supports iterate over enum (names and values) with `mgutility::enum_for_each<T>()` class and it is compatible with standard ranges and views

## Limitations
* Compiler versions
* Wider range can increase compile time so user responsible to adjusting for enum's range


## Usage ([try it!](https://godbolt.org/z/YM5EvY1Y5))
```C++
#include <iostream>
#include "enum_name.hpp"

num class rgb_color { red, green, blue, unknown = -1 };

// specialize rgb_color::unknown to make output "UNKNOWN" instead of "unknown"
template <>
constexpr inline auto mgutility::detail::enum_type::name<rgb_color, rgb_color::unknown>() noexcept
    -> string_view {
    return "UNKNOWN";
}

// you can specialize enum ranges with overload per enum types (option 1)
template <>
struct mgutility::enum_range<rgb_color> {
    static constexpr auto min = -1;
    static constexpr auto max = 3;
};

// you can specialize enum ranges with overload per enum types (option 2)
auto enum_name = [](rgb_color c) { return mgutility::enum_name<-1, 3>(c); };

#if defined(__cpp_lib_print)
#include <print>
#include <ranges>
#endif


int main() {
    auto x = rgb_color::blue;
    auto y = mgutility::to_enum<rgb_color>("greenn");

#if defined(__cpp_lib_print)

// enum_for_each<T> can usable with views and range algorithms
auto colors = mgutility::enum_for_each<rgb_color>() |
                  std::ranges::views::filter(
                      [](auto &&pair) { return pair.second != "UNKNOWN"; });

    std::ranges::for_each(colors, [](auto &&color) {
        std::println("{} \t: {}", color.second, mgutility::enum_to_underlying(color.first));
    });

#else

    for (auto&& e : mgutility::enum_for_each<rgb_color>()) {
        if(e.second != "UNKNOWN"){
            std::cout << e.second << " \t: " << mgutility::enum_to_underlying(e.first) << '\n';
        }
        // std::pair<Enum, string_view> {enum_type, name_of_enum}
    }
#endif

    // default signature: enum_name<min_value = -128, max_value = 128, Enum
    // typename>(Enum&&) Changing max_value to not too much greater than enum's
    // max value, it will compiles faster
    std::cout << mgutility::enum_name(x)
              << '\n';  // will print "blue" to output

    // calling specialized enum ranges function for rgb_color type
    // will print "green" to output, if y can't convert to rgb_color prints
    // "UNKNOWN"
    std::cout << enum_name(y.value_or(rgb_color::unknown)) << '\n';
}

```
