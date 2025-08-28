#include <print>
#include <concepts>
#include <utility>
#include <iostream>
#include <memory>

#include "parser.hpp"

using std::println;

void f(){
    std::string expr;
    std::getline(std::cin, expr);

    auto ret2 = ev::evaluate<double>(expr);

    if(ret2){
        println("Answer is {}", *ret2);
    }
    else{
        println("{}", ret2.error());
    }
}

void f2(){
    constexpr auto s = "1*10 / (3 / (2+1))";
    
    constexpr auto ret = ev::evaluate<double>(s);
    static_assert(ret.has_value());

    if(ret){
        println("\nAnswer is {}", *ret);
    }
    else{
        println("{}", ret.error());
    }
}

int main(){
    f();
    //f2();

    return 0;
}