#ifndef _MY_MATH_UTILS_
#define _MY_MATH_UTILS_

#include <concepts>

// useful when complex numbers will be a thing

namespace calc{
namespace math_utils{
    static constexpr double epsilon = 1.e-6;

    // struct complex{
    //     double real;
    //     double imm;
    // };
    // constexpr bool equal(const complex& v1, const complex& v2){
    //     return equal(v1.imm, v2.imm) && equal(v1.real, v2.real);
    // }
    // template<>
    // constexpr complex zero_element(){
    //     return {0, 0};
    // };
    
    template<typename T>
    requires std::is_arithmetic<T>::value
    constexpr T  zero_element(){
        return static_cast<T>(0);
    };
    
    template<std::integral T>
    constexpr bool equal(const T& v1, const T& v2){
        return v1 == v2;
    }

    template<std::floating_point T>
    constexpr bool equal(const T& v1, const T& v2){
        return std::fabs(v1 - v2) < epsilon;
    }

    template<typename T>
    constexpr bool is_zero(const T& v1){
        return equal(v1, zero_element<T>());
    }


}
}



#endif