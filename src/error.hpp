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

static constexpr size_t buf_sz = 127;

static constexpr void my_strncpy(char* dst, const char* src, ssize_t n){
    while(n-- && *src){
        *dst++ = *src++;
    }
}

namespace ev{
    enum class err_type_t{
        UNKNOWN_TOKEN,
        EMPTY_EXPRESSION,
        INVALID_LITERAL,
        EXPECTED_TOKEN,
        UNEXPECTED_TOKEN,
        INVALID_EXPR,
        NO_ERROR

    };

    struct calc_err{
        
        using enum err_type_t;
        
        err_type_t err_type;
        std::array<char, buf_sz + 1> err_msg;
        std::array<char, buf_sz + 1> wrong_expr;
        size_t start;
        size_t end;


        static constexpr calc_err error_message(err_type_t type, std::string_view msg){
            return calc_err(type, msg, "", 0, 0);
        }
        
        static constexpr calc_err no_error(){
            return calc_err(NO_ERROR, "", "", 0, 0);
        }

        static constexpr calc_err error_with_wrong_token(err_type_t type, std::string_view msg, std::string_view wrong, size_t start, size_t end){
            return calc_err(type, msg, wrong, start, end);
        }

private:
        constexpr calc_err(err_type_t type, std::string_view msg, std::string_view wrong, size_t s, size_t e):
            err_type(type), 
            start(s), 
            end(e)
        {
            err_msg.fill(0);
            wrong_expr.fill(0);

            my_strncpy(err_msg.data(), msg.data(), err_msg.max_size());
            my_strncpy(wrong_expr.data(), wrong.data(), wrong_expr.max_size());
        }

        static constexpr std::string err_to_string(ev::err_type_t err){
            using enum ev::err_type_t;
            switch(err){
                case UNKNOWN_TOKEN:
                    return "unknown token";
                case EMPTY_EXPRESSION:
                    return "empty expression";
                case INVALID_LITERAL:
                    return "invalid literal";
                case EXPECTED_TOKEN:
                    return "expected token";
                case UNEXPECTED_TOKEN:
                    return "unexpected token";
                case INVALID_EXPR:
                    return "invalid expression";
                
                default:
                    return "Unkown error";
            }
        };
    };
}


template <>
struct std::formatter<ev::calc_err>{

    constexpr auto parse(auto& ctx){
        auto it = ctx.begin();
        
        if (it != ctx.end() && *it != '}')
            throw std::format_error("Invalid format args");

        return it;
    }

    
    auto format(ev::calc_err s, auto& ctx) const {
        if(s.start == s.end){
            return std::format_to(
                ctx.out(), 
                RED "error:" RESET " {}", 
                s.err_msg.data()
            );
        }

        std::string wrong = s.wrong_expr.data();
        const auto wrong_part_1 = wrong.substr(0, s.start);
        const auto wrong_part_2 = wrong.substr(s.start, s.end - s.start);
        const auto wrong_part_3 = wrong.substr(s.end, wrong.length() - s.end);

        const auto spaces = std::string(s.start, ' ');
        const auto underline = std::string(s.end - s.start - 1, '~');
        return std::format_to(
            ctx.out(), 
            RED "error:" RESET " {}\n{}" RED "{}" RESET "{}\n{}" RED "^{}" RESET, 
            s.err_msg.data(), 
            wrong_part_1,
            wrong_part_2,
            wrong_part_3,
            spaces, 
            underline
        );
    }
};
#endif