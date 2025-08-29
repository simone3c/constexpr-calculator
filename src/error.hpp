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

namespace calc{
    enum class calc_err_type_t{
        UNKNOWN_TOKEN,
        EMPTY_EXPRESSION,
        INVALID_LITERAL,
        EXPECTED_TOKEN,
        UNEXPECTED_TOKEN,
        INVALID_EXPR,
        DIVISION_BY_ZERO,
        NO_ERROR
    };

    class calc_err{
        
        using enum calc_err_type_t;
        
        calc_err_type_t err_type;
        std::array<char, buf_sz + 1> err_msg;
        std::optional<std::array<char, buf_sz + 1>> wrong_expr;
        std::optional<size_t> start;
        std::optional<size_t> end;

    public:

        static constexpr calc_err error_message(
            calc_err_type_t type, 
            std::string_view msg
        ){
            return calc_err(type, msg, std::nullopt, std::nullopt, std::nullopt);
        }
        
        static constexpr calc_err no_error(){
            return calc_err(NO_ERROR, "No error", std::nullopt, std::nullopt, std::nullopt);
        }

        static constexpr calc_err error_with_wrong_token(
            calc_err_type_t type, 
            std::string_view msg, 
            std::string_view wrong, 
            size_t start, 
            size_t end
        ){
            return calc_err(type, msg, wrong, start, end);
        }

        constexpr calc_err_type_t get_err_type() const {
            return err_type;
        } 

        constexpr const auto& get_expr() const{
            return wrong_expr;
        }
        
        constexpr const auto& get_err_msg() const{
            return err_msg;
        }

        constexpr const auto& get_start() const{
            return start;
        }

        constexpr const auto& get_end() const{
            return end;
        }

    private:
        constexpr calc_err(
            calc_err_type_t type, 
            std::string_view msg, 
            std::optional<std::string_view> wrong, 
            std::optional<size_t> s, 
            std::optional<size_t> e
        ):
            err_type(type), 
            wrong_expr(std::nullopt),
            start(s), 
            end(e)
        {
            err_msg.fill(0);
            my_strncpy(err_msg.data(), msg.data(), err_msg.max_size());

            if(wrong){
                wrong_expr.emplace().fill(0);
                my_strncpy(wrong_expr->data(), wrong->data(), wrong_expr->max_size());
            }
        }
    };
}

template <>
struct std::formatter<calc::calc_err>{

    constexpr auto parse(auto& ctx){
        auto it = ctx.begin();
        
        if (it != ctx.end() && *it != '}')
            throw std::format_error("Invalid format args");

        return it;
    }
    
    auto format(calc::calc_err s, auto& ctx) const {
        if(!s.get_start().has_value()){
            return std::format_to(
                ctx.out(), 
                RED "error:" RESET " {}", 
                s.get_err_msg().data()
            );
        }

        // the fields used here must be present according to the class invariant
        // it's safe to dereference them
        std::string wrong = s.get_expr()->data();
        const auto wrong_part_1 = wrong.substr(0, *s.get_start());
        const auto wrong_part_2 = wrong.substr(*s.get_start(), *s.get_end() - *s.get_start());
        const auto wrong_part_3 = wrong.substr(*s.get_end(), wrong.length() - *s.get_end());

        const auto spaces = std::string(*s.get_start(), ' ');
        const auto underline = std::string(*s.get_end() - *s.get_start() - 1, '~');
        return std::format_to(
            ctx.out(), 
            RED "error:" RESET " {}\n{}" RED "{}" RESET "{}\n{}" RED "^{}" RESET, 
            s.get_err_msg().data(), 
            wrong_part_1,
            wrong_part_2,
            wrong_part_3,
            spaces, 
            underline
        );
    }
};
#endif