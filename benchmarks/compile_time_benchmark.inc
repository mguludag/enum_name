#if defined(ENUM_NAME_BENCHMARK_MGUTILITY)
#include "mgutility/reflection/enum_name.hpp"
#elif defined(ENUM_NAME_BENCHMARK_MAGIC_ENUM)
#include <magic_enum/magic_enum.hpp>
#elif defined(ENUM_NAME_BENCHMARK_ENCHANTUM)
#include <enchantum/enchantum.hpp>
#else
#error "A benchmark library must be selected."
#endif

namespace {

template <typename Enum>
constexpr auto enum_name(Enum value) {
#if defined(ENUM_NAME_BENCHMARK_MGUTILITY)
  return mgutility::enum_name(value);
#elif defined(ENUM_NAME_BENCHMARK_MAGIC_ENUM)
  return magic_enum::enum_name(value);
#else
  return enchantum::to_string(value);
#endif
}

template <typename Enum, int Start, int Count>
constexpr bool reflect_enum_values() {
  for (int value = Start; value < Start + Count; ++value) {
    if (enum_name(static_cast<Enum>(value)).empty()) {
      return false;
    }
  }
  return true;
}

} // namespace

#include "benchmark_enums.hpp"

int main() {}
