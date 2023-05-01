#include <iostream>
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

// you can specialize enum ranges with overload per enum types (option 2)
auto enum_name = [](rgb_color c){ return mgutility::enum_name<-1, 3>(c); };


int main()
{
    auto x = rgb_color::blue;
    auto y = mgutility::to_enum<rgb_color>("green");
    
    // default signature: enum_name<min_value = -128, max_value = 128, Enum typename>(Enum&&) 
    // Changing max_value to not too much greater than enum's max value, it will compiles faster
    std::cout << mgutility::enum_name(x) << '\n'; // will print "blue" to output
    
    // calling specialized enum ranges function for rgb_color type
    // will print "green" to output, if y can't convert to rgb_color prints "unknown"
    std::cout << enum_name(y.value_or(rgb_color::unknown)) << '\n'; 
}