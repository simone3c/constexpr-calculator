# constexpr-calculator
Constexpr-enabled interpreter of math expression developed in C++23

# Prerequisites
- g++ 14.2 or newer
- cmake 3.20 or newer

# Build & Run
```bash
    # clone
    > git clone https://github.com/simone3c/constexpr-calculator.git --recurse-submodules

    # build
    > cmake -B build
    > cmake --build build

    # run
    > build/main

```

# Usage
The evaluation can be performed at compile time
```c++
// constexpr usage

#include "parser.hpp"

int main(){
    constexpr auto expr = "1+1";
    constexpr auto val = ev::evaluate<int>(expr);

    static_assert(val.value() == 2);
}
```
but also at runtime if needed
```c++
// NON-contexpr usage

#include "parser.hpp"

int main(){
    auto expr = user_input();
    auto val = ev::evaluate<int>(expr);

    assert(val.value() == 2);
}
```

Errors can easily printed
```c++
#include <print>
#include "parser.hpp"

int main(){
    constexpr auto expr = "1+ error here"; 
    constexpr auto val = ev::evaluate<int>(expr);

    static_assert(!val.has_value() == 2);

    std::println("{}", val.error());
}
```

# TODO
- math functions
- exponent
- definite intgrals
- summation and product notation
- cmake option for not biulding tests