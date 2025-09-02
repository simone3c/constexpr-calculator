# constexpr-calculator
Constexpr-enabled interpreter of math expression developed in C++23

# Prerequisites
- g++ 14.2 or newer
- cmake 3.20 or newer

# Build
Your CMakeLists.txt should contain the following lines:
```
    add_subdirectory(path-to-constexpr-calculator) # external/.../constexpr-calculator

    add_executable(MyApp my-files-to-build) # main.cpp file1.cpp ...
    target_link_libraries(MyApp PRIVATE calc) # calc is the name of the library
```

```bash
    # clone
    > git clone https://github.com/simone3c/constexpr-calculator.git --recurse-submodules

    # build
    > cmake -B build -DBUILD_TESTS=OFF
    > cmake --build build

    # run
    > build/main

```

# Usage
The evaluation can be performed at compile time
```c++
// constexpr usage

#include "calculator.hpp"

int main(){
    constexpr auto expr = "1+1.5";
    constexpr auto val = ev::evaluate(expr);

    static_assert(val.value() == 2.5);
}
```
but also at runtime if needed
```c++
// NON-contexpr usage

#include "calculator.hpp"

int main(){
    auto expr = user_input();
    auto val = ev::evaluate(expr);

    assert(val.value() == 2);
}
```

Errors can be easily printed
```c++
#include <print>
#include "calculator.hpp"

int main(){
    constexpr auto expr = "1+ error here"; 
    constexpr auto val = ev::evaluate(expr);

    static_assert(!val.has_value());

    std::println("{}", val.error());
}
```

# TODO
- math functions
- definite intgrals
- summation and product notation
- change grammar to force writing "x-(-4)" insted of "x--4"
- complex numbers
- automatically deduce the evaluation type 