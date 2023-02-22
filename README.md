# enum_name
C++11 supported converting (scoped)enum values to string names.

## Supported Compilers
* Clang > 5
* GCC > 8
* MSVC > 2015

## Usage
```C++
#include <cstdio>

#define MIN_ENUM_VALUE -1


enum class rgb_color { red, green, blue, unknown = -1};


int main()
{
    auto x = rgb_color::blue;
    
    // default signature: enum_name<max_value = 256, Enum typename>(Enum&&) 
    // Changing max_value to not too much greater than enum's max value, it will compiles faster
    puts(mgutility::enum_name<10>(x).c_str()); // will print "rgb_color::blue" to output
}

```
