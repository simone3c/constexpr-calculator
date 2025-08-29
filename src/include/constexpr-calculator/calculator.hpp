#ifndef _MY_CALCULATOR_
#define _MY_CALCULATOR_

#include "parser.hpp"

namespace calc{

    template<typename eval_t>
    constexpr std::expected<eval_t, calc_err> evaluate(std::string_view str)
    requires std::is_arithmetic<eval_t>::value{
        return parser<eval_t>().evaluate(str);
    }
}

#endif