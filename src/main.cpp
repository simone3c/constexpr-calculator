#include <print>
#include <concepts>
#include <utility>
#include <iostream>
#include <memory>

#include "parser.hpp"

using std::println;


int main(){

    constexpr auto v = ev::evaluate<int>("1+1");
    static_assert(ev::evaluate<int>("1+1") == 2);
    // if(v){
    //     println("\nAnswer is {}", *v);
    // }

    return 0;
}