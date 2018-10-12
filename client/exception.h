#pragma once

#include <string>
#include <tuple>
#include <sstream>

/*
C++ 黑魔法，绕死人的变量参数
References:
https://en.cppreference.com/w/cpp/language/parameter_pack
https://stackoverflow.com/questions/6245735/pretty-print-stdtuple
https://stackoverflow.com/questions/12030538/calling-a-function-for-each-variadic-template-argument-and-an-array
*/

namespace bplan
{
    template<std::size_t...> struct seq {};

    template<std::size_t N, std::size_t... Is>
    struct gen_seq : gen_seq<N - 1, N - 1, Is...> {};

    template<std::size_t... Is>
    struct gen_seq<0, Is...> : seq<Is...> {};

    template<class Ch, class Tr, class Tuple, std::size_t... Is>
    void print_tuple(std::basic_ostream<Ch, Tr>& os, Tuple const& t, seq<Is...>) {
        using swallow = int[];
        (void)swallow {
            0, (void(os << std::get<Is>(t)), 0)...
        };
    }

    template <class... T>
    std::string tuple_to_str(std::tuple<T...> tuple)
    {
        std::ostringstream oss;
        print_tuple(oss, tuple, gen_seq<sizeof...(T)>());
        return oss.str();
    }

    template<typename... Args>
    void thrExpt(Args... args)
    {
        std::string str = tuple_to_str(std::tie(args...));
        throw std::exception(str.c_str());
    }
} // namespace bplan
