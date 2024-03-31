#include "mgutility/reflection/enum_name.hpp"
#include <iostream>

enum class rgb_color {
  red = 1 << 0,
  green = 1 << 1,
  blue = 1 << 2,
  unknown = -1
};

auto constexpr operator|(rgb_color lhs, rgb_color rhs) -> rgb_color {
  return static_cast<rgb_color>(mgutility::enum_to_underlying(lhs) |
                                mgutility::enum_to_underlying(rhs));
}

auto constexpr operator&(rgb_color lhs, rgb_color rhs) -> rgb_color {
  return static_cast<rgb_color>(mgutility::enum_to_underlying(lhs) &
                                mgutility::enum_to_underlying(rhs));
}

// specialize rgb_color::unknown to make output "UNKNOWN" instead of "unknown"
template <>
constexpr auto
mgutility::detail::enum_type::name<rgb_color, rgb_color::unknown>() noexcept
    -> mgutility::string_view {
  return "UNKNOWN";
}

// you can specialize enum ranges with overload per enum types (option 1)
template <> struct mgutility::enum_range<rgb_color> {
  static constexpr auto min = -1;
  static constexpr auto max = 5;
};

// you can specialize enum ranges with overload per enum types (option 2)
auto enum_name = [](rgb_color c) { return mgutility::enum_name<-1, 5>(c); };

#if defined(__cpp_lib_print)
#include <print>
#include <ranges>
#endif

int main() {
  auto x = rgb_color::blue;
  auto y = mgutility::to_enum<rgb_color>("green|red");

#if defined(__cpp_lib_print)

  // enum_for_each<T> can usable with views and range algorithms
  auto colors = mgutility::enum_for_each<rgb_color>() |
                std::ranges::views::filter([](auto &&pair) {
                  return !pair.second.empty() && pair.second != "UNKNOWN";
                });

  std::ranges::for_each(colors, [](auto &&color) {
    std::println("{} \t: {}", color.second,
                 mgutility::enum_to_underlying(color.first));
  });

#else

  for (auto &&e : mgutility::enum_for_each<rgb_color>()) {
    if (!e.second.empty() && e.second != "UNKNOWN") {
      std::cout << e.second << " \t: " << mgutility::enum_to_underlying(e.first)
                << '\n';
    }
    // std::pair<Enum, string_view> {enum_type, name_of_enum}
  }
#endif

  // default signature: enum_name<min_value = -128, max_value = 128, Enum
  // typename>(Enum&&) Changing max_value to not too much greater than enum's
  // max value, it will compiles faster
  std::cout << x << '\n'; // will print "blue" to output

  // calling specialized enum ranges function for rgb_color type
  // will print "green" to output, if y can't convert to rgb_color prints
  // "UNKNOWN"
  std::cout << enum_name(y.value_or(rgb_color::unknown)) << '\n';
}