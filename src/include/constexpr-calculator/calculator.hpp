#ifndef _MY_CALCULATOR_
#define _MY_CALCULATOR_

#include "parser.hpp"

namespace calc{

    constexpr evaluation_t evaluate(std::string_view str){
        return parser().evaluate(str);
    }
}

#endif