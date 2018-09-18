#include "stdafx.h"
#include "hex.h"

#include <sstream>

namespace bplan
{
    /*
    copy from libtorrent
    */

    namespace detail {

        int hex_to_int(char in)
        {
            if (in >= '0' && in <= '9') return int(in) - '0';
            if (in >= 'A' && in <= 'F') return int(in) - 'A' + 10;
            if (in >= 'a' && in <= 'f') return int(in) - 'a' + 10;
            return -1;
        }

        bool is_hex(char const *in, int len)
        {
            for (char const* end = in + len; in < end; ++in)
            {
                int t = hex_to_int(*in);
                if (t == -1) return false;
            }
            return true;
        }

    } // detail namespace

    std::string from_hex(std::string const& s)
    {
        std::ostringstream oss;
        for (std::string::const_iterator it = s.begin(); it != s.end(); ++it)
        {
            char out;
            int t = detail::hex_to_int(*it);
            if (t == -1) return "";
            out = t << 4;
            ++it;
            t = detail::hex_to_int(*it);
            if (t == -1) return "";
            out |= t & 15;
            oss << out;
        }
        return oss.str();
    }

    bool from_hex(char const *in, int len, char* out)
    {
        for (char const* end = in + len; in < end; ++in, ++out)
        {
            int t = detail::hex_to_int(*in);
            if (t == -1) return false;
            *out = t << 4;
            ++in;
            t = detail::hex_to_int(*in);
            if (t == -1) return false;
            *out |= t & 15;
        }
        return true;
    }

    extern const char hex_chars[];

    const char hex_chars[] = "0123456789abcdef";

    std::string to_hex(std::string const& s)
    {
        std::string ret;
        for (std::string::const_iterator i = s.begin(); i != s.end(); ++i)
        {
            ret += hex_chars[std::uint8_t(*i) >> 4];
            ret += hex_chars[std::uint8_t(*i) & 0xf];
        }
        return ret;
    }

    void to_hex(char const *in, int len, char* out)
    {
        for (char const* end = in + len; in < end; ++in)
        {
            *out++ = hex_chars[std::uint8_t(*in) >> 4];
            *out++ = hex_chars[std::uint8_t(*in) & 0xf];
        }
        *out = '\0';
    }

} // namespace bplan
