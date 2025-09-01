#ifndef _MY_CALCULATOR_
#define _MY_CALCULATOR_

#include "parser.hpp"

namespace calc{

    template<typename num_t>
    constexpr evaluation_t<num_t> evaluate(std::string_view str)
    requires std::is_signed<num_t>::value{
        return parser<num_t>().evaluate(str);
    }
}

#endif