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
* Changing enum range with template parameter <sub>(default range: `[0, 256)`)</sub> on each call or with your special function for types
* Supports `operator<<` for direct using with ostream objects

## Limitations
* Compiler versions
* Wider range can increase compile time so user responsible to adjusting for enum's range
* Enums in templated classes/structs and if its template parameter is templated type <sub>example: `std::string`</sub> some compilers/versions can produce longer names for it.


## Usage ([try it!](https://godbolt.org/z/76jYG3aWh))
```C++
#include <cstdio>
#include <enum_name.hpp>


enum class rgb_color { red, green, blue, unknown = -1};

// you can specialize enum ranges with overload per enum types
auto enum_name = [](rgb_color c){ return mgutility::enum_name<-1, 10>(c); };


int main()
{
    auto x = rgb_color::blue;
    auto y = mgutility::to_enum_simple<rgb_color, -1, 3>("green");
    
    // default signature: enum_name<min_value = 0, max_value = 256, Enum typename>(Enum&&) 
    // Changing max_value to not too much greater than enum's max value, it will compiles faster
    puts(mgutility::enum_name<-1, 3>(x).c_str()); // will print "rgb_color::blue" to output
    puts(mgutility::enum_name_simple<-1, 3>(x).c_str()); // will print "blue" to output
    
    // calling specialized enum ranges function for rgb_color type
    // will print "rgb_color::green" to output, if y can't convert to rgb_color prınts "rgb_color::unknown"
    puts(enum_name(y.value_or(rgb_color::unknown)).c_str()); 
}

```
