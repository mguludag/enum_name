#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "enum_name.hpp"

enum class rgb_color { red, green, blue, unknown = -1};

// you can specialize enum ranges with specialize struct per enum types (option 1)
namespace mgutility{
    template<>
    struct enum_range<rgb_color>
    {
        static constexpr auto min = -1;
        static constexpr auto max = 3;
    };
}


TEST_CASE("testing the enum name serialization") {
    CHECK(mgutility::enum_name(rgb_color::blue) == "blue");
    CHECK(mgutility::enum_name(rgb_color::unknown) == "unknown");
    CHECK(mgutility::enum_name<0, 2>(rgb_color::unknown) == "");
}

TEST_CASE("testing the enum name deserialization") {
    CHECK(mgutility::to_enum<rgb_color>("blue").value() == rgb_color::blue);
    CHECK(mgutility::to_enum<rgb_color>("unknown").value() == rgb_color::unknown);
    CHECK(mgutility::to_enum<rgb_color, 0, 2>("unknown").value_or(rgb_color::red) == rgb_color::red);
    REQUIRE_THROWS(mgutility::to_enum<rgb_color, 0, 2>("unknown").value());
}