# enum_name
C++11 supported converting (scoped)enum values to string names.

## Supported Compilers
* Clang > 5
* GCC > 8
* MSVC > 2015

## Usage ([try it!](https://godbolt.org/z/x717sK7s8))
```C++
#include <cstdio>
#include <enum_name.hpp>


enum class rgb_color { red, green, blue, unknown = -1};

// you can specialize enum ranges with overload per enum types
inline auto enum_name(rgb_color c) -> std::string {
    return mgutility::enum_name<-1, 10>(c);
}


int main()
{
    auto x = rgb_color::blue;
    
    // default signature: enum_name<min_value = 0, max_value = 256, Enum typename>(Enum&&) 
    // Changing max_value to not too much greater than enum's max value, it will compiles faster
    puts(mgutility::enum_name<-1, 10>(x).c_str()); // will print "rgb_color::blue" to output
    
    // calling specialized enum ranges function for rgb_color type
    puts(enum_name(x).c_str()); // will print "rgb_color::blue" to output
}

```
