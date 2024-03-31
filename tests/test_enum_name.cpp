#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "mgutility/reflection/enum_name.hpp"

enum class color : uint32_t {
  alice_blue,              // rgb(240,248,255)
  antique_white,           // rgb(250,235,215)
  aqua,                    // rgb(0,255,255)
  aquamarine,              // rgb(127,255,212)
  azure,                   // rgb(240,255,255)
  beige,                   // rgb(245,245,220)
  bisque,                  // rgb(255,228,196)
  black,                   // rgb(0,0,0)
  blanched_almond,         // rgb(255,235,205)
  blue,                    // rgb(0,0,255)
  blue_violet,             // rgb(138,43,226)
  brown,                   // rgb(165,42,42)
  burly_wood,              // rgb(222,184,135)
  cadet_blue,              // rgb(95,158,160)
  chartreuse,              // rgb(127,255,0)
  chocolate,               // rgb(210,105,30)
  coral,                   // rgb(255,127,80)
  cornflower_blue,         // rgb(100,149,237)
  cornsilk,                // rgb(255,248,220)
  crimson,                 // rgb(220,20,60)
  cyan,                    // rgb(0,255,255)
  dark_blue,               // rgb(0,0,139)
  dark_cyan,               // rgb(0,139,139)
  dark_golden_rod,         // rgb(184,134,11)
  dark_gray,               // rgb(169,169,169)
  dark_green,              // rgb(0,100,0)
  dark_khaki,              // rgb(189,183,107)
  dark_magenta,            // rgb(139,0,139)
  dark_olive_green,        // rgb(85,107,47)
  dark_orange,             // rgb(255,140,0)
  dark_orchid,             // rgb(153,50,204)
  dark_red,                // rgb(139,0,0)
  dark_salmon,             // rgb(233,150,122)
  dark_sea_green,          // rgb(143,188,143)
  dark_slate_blue,         // rgb(72,61,139)
  dark_slate_gray,         // rgb(47,79,79)
  dark_turquoise,          // rgb(0,206,209)
  dark_violet,             // rgb(148,0,211)
  deep_pink,               // rgb(255,20,147)
  deep_sky_blue,           // rgb(0,191,255)
  dim_gray,                // rgb(105,105,105)
  dodger_blue,             // rgb(30,144,255)
  fire_brick,              // rgb(178,34,34)
  floral_white,            // rgb(255,250,240)
  forest_green,            // rgb(34,139,34)
  fuchsia,                 // rgb(255,0,255)
  gainsboro,               // rgb(220,220,220)
  ghost_white,             // rgb(248,248,255)
  gold,                    // rgb(255,215,0)
  golden_rod,              // rgb(218,165,32)
  gray,                    // rgb(128,128,128)
  green,                   // rgb(0,128,0)
  green_yellow,            // rgb(173,255,47)
  honey_dew,               // rgb(240,255,240)
  hot_pink,                // rgb(255,105,180)
  indian_red,              // rgb(205,92,92)
  indigo,                  // rgb(75,0,130)
  ivory,                   // rgb(255,255,240)
  khaki,                   // rgb(240,230,140)
  lavender,                // rgb(230,230,250)
  lavender_blush,          // rgb(255,240,245)
  lawn_green,              // rgb(124,252,0)
  lemon_chiffon,           // rgb(255,250,205)
  light_blue,              // rgb(173,216,230)
  light_coral,             // rgb(240,128,128)
  light_cyan,              // rgb(224,255,255)
  light_golden_rod_yellow, // rgb(250,250,210)
  light_gray,              // rgb(211,211,211)
  light_green,             // rgb(144,238,144)
  light_pink,              // rgb(255,182,193)
  light_salmon,            // rgb(255,160,122)
  light_sea_green,         // rgb(32,178,170)
  light_sky_blue,          // rgb(135,206,250)
  light_slate_gray,        // rgb(119,136,153)
  light_steel_blue,        // rgb(176,196,222)
  light_yellow,            // rgb(255,255,224)
  lime,                    // rgb(0,255,0)
  lime_green,              // rgb(50,205,50)
  linen,                   // rgb(250,240,230)
  magenta,                 // rgb(255,0,255)
  maroon,                  // rgb(128,0,0)
  medium_aquamarine,       // rgb(102,205,170)
  medium_blue,             // rgb(0,0,205)
  medium_orchid,           // rgb(186,85,211)
  medium_purple,           // rgb(147,112,219)
  medium_sea_green,        // rgb(60,179,113)
  medium_slate_blue,       // rgb(123,104,238)
  medium_spring_green,     // rgb(0,250,154)
  medium_turquoise,        // rgb(72,209,204)
  medium_violet_red,       // rgb(199,21,133)
  midnight_blue,           // rgb(25,25,112)
  mint_cream,              // rgb(245,255,250)
  misty_rose,              // rgb(255,228,225)
  moccasin,                // rgb(255,228,181)
  navajo_white,            // rgb(255,222,173)
  navy,                    // rgb(0,0,128)
  old_lace,                // rgb(253,245,230)
  olive,                   // rgb(128,128,0)
  olive_drab,              // rgb(107,142,35)
  orange,                  // rgb(255,165,0)
  orange_red,              // rgb(255,69,0)
  orchid,                  // rgb(218,112,214)
  pale_golden_rod,         // rgb(238,232,170)
  pale_green,              // rgb(152,251,152)
  pale_turquoise,          // rgb(175,238,238)
  pale_violet_red,         // rgb(219,112,147)
  papaya_whip,             // rgb(255,239,213)
  peach_puff,              // rgb(255,218,185)
  peru,                    // rgb(205,133,63)
  pink,                    // rgb(255,192,203)
  plum,                    // rgb(221,160,221)
  powder_blue,             // rgb(176,224,230)
  purple,                  // rgb(128,0,128)
  rebecca_purple,          // rgb(102,51,153)
  red,                     // rgb(255,0,0)
  rosy_brown,              // rgb(188,143,143)
  royal_blue,              // rgb(65,105,225)
  saddle_brown,            // rgb(139,69,19)
  salmon,                  // rgb(250,128,114)
  sandy_brown,             // rgb(244,164,96)
  sea_green,               // rgb(46,139,87)
  sea_shell,               // rgb(255,245,238)
  sienna,                  // rgb(160,82,45)
  silver,                  // rgb(192,192,192)
  sky_blue,                // rgb(135,206,235)
  slate_blue,              // rgb(106,90,205)
  slate_gray,              // rgb(112,128,144)
  snow,                    // rgb(255,250,250)
  spring_green,            // rgb(0,255,127)
  steel_blue,              // rgb(70,130,180)
  tan,                     // rgb(210,180,140)
  teal,                    // rgb(0,128,128)
  thistle,                 // rgb(216,191,216)
  tomato,                  // rgb(255,99,71)
  turquoise,               // rgb(64,224,208)
  violet,                  // rgb(238,130,238)
  wheat,                   // rgb(245,222,179)
  white,                   // rgb(255,255,255)
  white_smoke,             // rgb(245,245,245)
  yellow,                  // rgb(255,255,0)
  yellow_green             // rgb(154,205,50)
};                         // enum class color

namespace mgutility {
template <> struct enum_range<color> {
  static constexpr auto min = 0;
  static constexpr auto max = 150;
};
} // namespace mgutility

TEST_CASE("testing the enum name serialization") {
  CHECK(mgutility::enum_name(color::blue) == "blue");
  CHECK(mgutility::enum_name(color::white_smoke) == "white_smoke");
  CHECK(mgutility::enum_name(color::yellow_green) == "yellow_green");
  CHECK(mgutility::enum_name(color::steel_blue) == "steel_blue");
  CHECK(mgutility::enum_name(color::pale_turquoise) == "pale_turquoise");
  CHECK(mgutility::enum_name(color::white_smoke) == "white_smoke");
  CHECK(mgutility::enum_name(color::light_golden_rod_yellow) ==
        "light_golden_rod_yellow");
  CHECK(mgutility::enum_name(color::dark_olive_green) == "dark_olive_green");
  CHECK(mgutility::enum_name(color::azure) == "azure");
  CHECK(mgutility::enum_name(color::ghost_white) == "ghost_white");
  CHECK(mgutility::enum_name(color::maroon) == "maroon");
  CHECK(mgutility::enum_name(color::navy) == "navy");
  CHECK(mgutility::enum_name(color::spring_green) == "spring_green");
  CHECK(mgutility::enum_name(color::rebecca_purple) == "rebecca_purple");
  CHECK(mgutility::enum_name(color::red) == "red");
  CHECK(mgutility::enum_name(color::tan) == "tan");
  CHECK(mgutility::enum_name(color::mint_cream) == "mint_cream");
  CHECK(mgutility::enum_name(color::light_green) == "light_green");
  CHECK(mgutility::enum_name(color::dodger_blue) == "dodger_blue");
  CHECK(mgutility::enum_name(color::cornflower_blue) == "cornflower_blue");
  CHECK(mgutility::enum_name<-5, 0>(color::thistle) == "");
}

TEST_CASE("testing the enum name deserialization") {
  CHECK(mgutility::to_enum<color>("blue").value() == color::blue);
  CHECK(mgutility::to_enum<color>("white_smoke").value() == color::white_smoke);
  CHECK(mgutility::to_enum<color>("yellow_green").value() ==
        color::yellow_green);
  CHECK(mgutility::to_enum<color>("steel_blue").value() == color::steel_blue);
  CHECK(mgutility::to_enum<color>("pale_turquoise").value() ==
        color::pale_turquoise);
  CHECK(mgutility::to_enum<color>("white_smoke").value() == color::white_smoke);
  CHECK(mgutility::to_enum<color>("light_golden_rod_yellow").value() ==
        color::light_golden_rod_yellow);
  CHECK(mgutility::to_enum<color>("dark_olive_green").value() ==
        color::dark_olive_green);
  CHECK(mgutility::to_enum<color>("azure").value() == color::azure);
  CHECK(mgutility::to_enum<color>("ghost_white").value() == color::ghost_white);
  CHECK(mgutility::to_enum<color>("maroon").value() == color::maroon);
  CHECK(mgutility::to_enum<color>("navy").value() == color::navy);
  CHECK(mgutility::to_enum<color>("spring_green").value() ==
        color::spring_green);
  CHECK(mgutility::to_enum<color>("rebecca_purple").value() ==
        color::rebecca_purple);
  CHECK(mgutility::to_enum<color>("red").value() == color::red);
  CHECK(mgutility::to_enum<color>("tan").value() == color::tan);
  CHECK(mgutility::to_enum<color>("mint_cream").value() == color::mint_cream);
  CHECK(mgutility::to_enum<color>("light_green").value() == color::light_green);
  CHECK(mgutility::to_enum<color>("dodger_blue").value() == color::dodger_blue);
  CHECK(mgutility::to_enum<color>("cornflower_blue").value() ==
        color::cornflower_blue);
  REQUIRE_THROWS(mgutility::to_enum<color, -5, 0>("thistle").value());
}