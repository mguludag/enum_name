# enum_name
C++11 supported converting (scoped)enum values to string names.

## Supported Compilers
* Clang > 5
* GCC > 8
* MSVC > 2015

## Usage
```C++
#include <cstdio>

// opt 1: define range
// you can define min-max enum range for global 
#define MIN_ENUM_VALUE -1
#define MAX_ENUM_VALUE 10

#include "enum_name.hpp"


enum class rgb_color { red, green, blue, unknown = -1};

// opt 2: define range
// you can define min-max enum range for per enum type
ENUM_RANGE(rgb_color, -1, 10)


int main()
{
    auto x = rgb_color::blue;
    
    // opt 3: define range but you can only change enum max range for each func call
    // default signature: enum_name<int max_value, Enum typename>(Enum&&) 
    // Changing max_value to not too much greater than enum's max value, it will compiles faster
    puts(mgutility::enum_name<10>(x).c_str()); // will print "rgb_color::blue" to output
    
    // print enum name using predefined [0, 256) range or defined opt 1 or 2 ranges
    puts(mgutility::enum_name(x).c_str()); // will print "rgb_color::blue" to output
}

```
