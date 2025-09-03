#ifndef _MY_MATH_UTILS_
#define _MY_MATH_UTILS_

#include <concepts>
#include <limits>

namespace calc{
    using num_t = double;

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
    constexpr T zero_element(){
        return static_cast<T>(0);
    };

    constexpr bool equal(num_t v1, num_t v2){
        return std::fabs(v1 - v2) < epsilon;
    }

    constexpr bool is_zero(num_t v1){
        return equal(v1, zero_element<num_t>());
    }

    constexpr num_t remove_decimal_part(num_t v){
        return std::round(v);
    }

    constexpr bool is_integer(num_t v1){
        return equal(v1, remove_decimal_part(v1));
    }

    constexpr std::optional<num_t> safe_add(num_t l, num_t r){
        const auto ret = l + r;
        if(!std::isfinite(ret)){
            return std::nullopt;
        }

        return ret;
    }

    constexpr std::optional<num_t> safe_sub(num_t l, num_t r){
        return safe_add(l, static_cast<num_t>(-r));
    }

    constexpr std::optional<num_t> safe_mult(num_t l, num_t r){
        const auto ret = l * r;
        if(!std::isfinite(ret)){
            return std::nullopt;
        }

        return ret;
    }

    constexpr std::optional<num_t> safe_div(num_t l, num_t r){
        const auto ret = l / r;
        if(!std::isfinite(ret)){
            return std::nullopt;
        }

        return ret;
    }

}
}



#endif