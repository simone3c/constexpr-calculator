#ifndef _MY_TOKENIZER_
#define _MY_TOKENIZER_

#include <string>
#include <string_view>
#include <vector>
#include <print>
#include <algorithm>
#include <cctype>
#include <optional>
#include <cstring>
#include <utility>

#include "ctre/single-header/ctre.hpp"

using std::println;
using std::print;

#define RED   "\x1B[1;31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"
#define regex "(?<lit>[0-9]+)|(?<open_par>\\()|(?<closed_par>\\))|(?<plus>\\+)|(?<minus>-)|(?<exponent>^)|(?<space>\\s+)|.+"


struct tokenizer{

    enum class TOKEN_TYPE{
        LIT,
        OPEN_PAR,
        CLOSED_PAR,
        PLUS,
        MINUS,
        EXPONENT,
    };

    struct token{
        TOKEN_TYPE type;
        std::string val;
    };

    std::vector<token> tokens;
    std::string str;

    constexpr tokenizer() = default;

    constexpr tokenizer(std::string_view input): tokens({}), str(input){
        tokenize(input);
    }

    constexpr void error_log(int i, std::string_view err, std::string_view wrong_token){
        int underline_len = (wrong_token.size() > 0) * wrong_token.size() - 1;

        println("\n" RED "error:" RESET " {} '{}'\n{}\n{}" RED "^{}" RESET, 
            err, 
            wrong_token, 
            str, 
            std::string(i, ' '),
            std::string(underline_len, '~')
        );
    }

    constexpr bool tokenize(std::string_view input){
        tokens.clear();
        str.clear();
        int i = 0;

        if(input.size() == 0){
            error_log(i, "Empty input", "insert at least one character");
            return false;
        }

        str = input;

        auto res = ctre::tokenize<regex>(str);
        for(const auto& r : res){
            token t;

            i += r.str().size();

            if(r.get<"lit">()){
                t.type = TOKEN_TYPE::LIT;
            }
            else if(r.get<"open_par">()){
                t.type = TOKEN_TYPE::OPEN_PAR;
            }
            else if(r.get<"closed_par">()){
                t.type = TOKEN_TYPE::CLOSED_PAR;
            }
            else if(r.get<"plus">()){
                t.type = TOKEN_TYPE::PLUS;
            }
            else if(r.get<"minus">()){
                t.type = TOKEN_TYPE::MINUS;
            }
            else if(r.get<"space">()){
                continue;
            }
            else{
                i -= r.str().size();
                error_log(i, "Unknown symbol", r.str());
                // clear internal status
                tokens.clear();
                str.clear();

                return false;
            }
            t.val = r.str();
            tokens.push_back(t);
            println("Pushed '{}'", r.str());

        }

        return true;
    }

    [[nodiscard]] constexpr std::optional<token> peek() const {
        if(tokens.empty())
            return std::nullopt;

        return tokens.front();
    }

    [[maybe_unused]] constexpr std::optional<token> next(){
        auto ret = peek();
        if(ret){
            tokens.erase(tokens.begin());
            println("Popped '{}'", ret->val);
        }


        return ret;
    }

    [[nodiscard]] constexpr bool match(TOKEN_TYPE other) const {
        return peek().transform([other](const token& tok){return tok.type == other;}).value_or(false);
    }

    [[nodiscard]] constexpr bool consume(TOKEN_TYPE other) {
        auto ret = match(other);
        next();
        return ret;
    }

    [[nodiscard]] constexpr bool has_more_tokens() const {
        return tokens.size() > 0;
    }
};


#endif