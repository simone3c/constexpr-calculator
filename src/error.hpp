#ifndef _MY_ERROR_
#define _MY_ERROR_

#include <string>
#include <format>
#include <cstring>

#define RED   "\x1B[1;31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

namespace ev{
    struct error{
        enum class err_type_t{
            UNKNOWN_TOKEN,
            EMPTY_EXPRESSION,
            INVALID_LITERAL,
            EXPECTED_TOKEN,
            EMPTY_PAR,
            UNEXPECTED_TOKEN,
            INVALID_EXPR,
            NO_ERROR

        };
        using enum err_type_t;
        
        err_type_t err_type;
        std::array<char, 65> err_msg;
        std::array<char, 257> wrong_expr;
        size_t start;
        size_t end;

        static constexpr error basic_error(err_type_t type, std::string_view msg){
            return error(type, msg, "", 0, 0);
        }
        
        static constexpr error no_error(){
            return error(NO_ERROR, "", "", 0, 0);
        }

        constexpr error(err_type_t type, std::string_view msg, std::string wrong, size_t s, size_t e):
            err_type(type), 
            start(s), 
            end(e)
        {
            err_msg.fill(0);
            wrong_expr.fill(0);

            //TODO fix copy logic
            if(msg.size() > 0)
                for(int i = 0; (i < msg.size() - 1) && msg[i]; ++i)
                    err_msg[i] = msg[i];

            if(wrong.size() > 0)
                for(int i = 0; (i < wrong.size() - 1) && wrong[i]; ++i)
                    wrong_expr[i] = wrong[i];
        }

        static constexpr std::string err_to_string(error::err_type_t err){
            using enum error::err_type_t;
            switch(err){
                case UNKNOWN_TOKEN:
                    return "unknown token";
                case EMPTY_EXPRESSION:
                    return "empty expression";
                case INVALID_LITERAL:
                    return "invalid literal";
                case EXPECTED_TOKEN:
                    return "expected token";
                case EMPTY_PAR:
                    return "empty parenthesis";
                case UNEXPECTED_TOKEN:
                    return "unexpected token";
                case INVALID_EXPR:
                    return "invalid expression";
                
                default:
                    return "Unkown error";
            }
        };
    };   

    constexpr void print_error(const ev::error& err){
        // if(std::is_constant_evaluated()){
        //     throw err.err_msg;
        // }
        // else{
            std::println("{}", err.err_msg.data());
        //}
    }

}

template<>
struct std::formatter<ev::error>{

    constexpr auto parse(auto& ctx){
        auto it = ctx.begin();
        
        if (it != ctx.end() && *it != '}')
            throw std::format_error("Invalid format args");

        return it;
    }

    
    auto format(ev::error s, auto& ctx) const {
        if(s.start == s.end && s.start == 0){
            return std::format_to(
                ctx.out(), 
                RED "error:" RESET " {}", 
                s.err_msg
            );
        }

        auto spaces = std::string(s.start, ' ');
        auto underline = std::string(s.end - s.start - 1, '~');
        return std::format_to(
            ctx.out(), 
            RED "error:" RESET " {}\n{}\n{}^{}", 
            s.err_msg.data(), 
            s.wrong_expr.data(), 
            spaces, 
            underline
        );
    }
};
#endif