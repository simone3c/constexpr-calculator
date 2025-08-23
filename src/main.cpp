#include <print>
#include <concepts>
#include <utility>
#include <iostream>
#include <memory>

#include "parser.hpp"

using std::println;

using evaluation_t = int;

constexpr std::expected<evaluation_t, std::string> run(){

    std::string expr;
    std::getline(std::cin, expr);

    if(auto r = ev::evaluate<int>(expr)){
        return *r;
    }
    return std::unexpected("Result not available");
}

int main(){

    auto v = run();
    //static_assert(v == 108);
    if(v){
        println("\nAnswer is {}", *v);
    }
    else{
        println("\nError is {}", v.error());
    }

    return 0;
}