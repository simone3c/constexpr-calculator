#ifndef _MY_MATH_UTILS_
#define _MY_MATH_UTILS_

#include <concepts>
#include <limits>

// useful when complex numbers will be a thing

namespace calc{
namespace math_utils{
    static constexpr double epsilon = 1.e-12;

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
    requires std::is_signed<T>::value
    constexpr T  zero_element(){
        return static_cast<T>(0);
    };
    
    template<std::integral T>
    constexpr bool equal(T v1, T v2){
        return v1 == v2;
    }

    template<std::floating_point T>
    constexpr bool equal(T v1, T v2){
        return std::fabs(v1 - v2) < epsilon;
    }

    template<typename T>
    requires std::is_signed<T>::value
    constexpr bool is_zero(T v1){
        return equal(v1, zero_element<T>());
    }

    template<typename T>
    requires std::is_signed<T>::value
    constexpr std::optional<T> safe_add(T l, T r){
        if(r > 0 && l > std::numeric_limits<T>::max() - r){
            return std::nullopt;
        }

        if(r < 0 && l < std::numeric_limits<T>::min() - r){
            return std::nullopt;
        }

        return l + r;
    }

    template<typename T>
    requires std::is_signed<T>::value
    constexpr std::optional<T> safe_sub(T l, T r){

        return safe_add(l, static_cast<T>(-r));
        
        if(r > 0 && l < std::numeric_limits<T>::min() + r){
            return std::nullopt;
        }

        if(r < 0 && l > std::numeric_limits<T>::max() + r){
            return std::nullopt;
        }

        return l - r;
    }

    template<typename T>
    requires std::is_signed<T>::value
    constexpr std::optional<T> safe_mult(T l, T r){
        if(r > 0 && l > 0 && l > std::numeric_limits<T>::max() / r){
            return std::nullopt;
        }
        
        if(r < 0 && l < 0 && l < std::numeric_limits<T>::max() / r){
            return std::nullopt;
        }

        if(r > 0 && l < 0 && l < std::numeric_limits<T>::min() / r){
            return std::nullopt;
        }

        if(r < 0 && l > 0 && l > std::numeric_limits<T>::min() / r){
            return std::nullopt;
        }

        return l * r;
    }


}
}



#endif