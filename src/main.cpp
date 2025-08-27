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

    auto ret = ev::evaluate<int>(expr);

    if(ret){
        println("Answer is {}", *ret);
    }
    else{
        println("{}", ret.error());
    }
}

void f2(){
    constexpr std::string_view s = "1+997";
    
    constexpr auto ret = ev::evaluate<int>(s);
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