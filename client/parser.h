#pragma once

#include <iomanip>
#include <string>
#include <sstream>

#include <utf8.h>
#include <nowide/iostream.hpp>

using namespace std;
namespace parser {
    string toUTF8(const wstring &s)
    {
        std::string utf8_str;
        int len = WideCharToMultiByte(CP_UTF8, 0,
            s.c_str(), (int)s.length(),
            NULL, 0, NULL, NULL);
        if (len > 0)
        {
            utf8_str.resize(len);
            WideCharToMultiByte(CP_UTF8, 0,
                s.c_str(), (int)s.length(), &utf8_str[0], 
                len, NULL, NULL);
        }
        return utf8_str;
    }

    wstring toWide(const string& s)
    {
        int size = MultiByteToWideChar(CP_ACP, MB_COMPOSITE,
            s.c_str(), (int)s.length(),
            nullptr, 0);

        std::wstring utf16_str(size, '\0');
        MultiByteToWideChar(CP_ACP, MB_COMPOSITE,
            s.c_str(), (int)s.length(),
            &utf16_str[0], size);
        return utf16_str;
    }

    string toUTF8(const string& s)
    {
        return nowide::narrow(toWide(s));
    }

    namespace url
    {
        bool dontNeedEncoding(char c)
        {
            int i = (int)c;
            if (i >= '0' && i <= '9')
                return true;

            if (i >= 'a' && i <= 'z')
                return true;

            if (i >= 'A' && i <= 'Z')
                return true;

            if (i == '-' || i == '_' || i == '.' || i == '~')
                return true;

            return false;
        }

        /*
        先将 string 转成 UTF8 存储，再进行 URL 编码
        */
        string encode(const string& s)
        {
            string u8s = s;
            string::iterator end_it = utf8::find_invalid(u8s.begin(), u8s.end());
            if (end_it != u8s.end()) { // not utf8
                u8s = toUTF8(u8s);     // convert to utf8
            }

            ostringstream escaped;
            escaped.fill('0');
            escaped << hex;

            for (string::const_iterator i = u8s.begin(), n = u8s.end(); i != n; ++i) {
                string::value_type c = (*i);

                // Keep alphanumeric and other accepted characters intact
                if (dontNeedEncoding(c)) {
                    escaped << c;
                    continue;
                }

                // Any other characters are percent-encoded
                escaped << uppercase;
                escaped << '%' << (int)(unsigned char)c;
                escaped << nouppercase;
            }

            return escaped.str();
        }

        string decode(const string& value)
        {
            ostringstream ss;
            // TODO: 实现
            return ss.str();
        }
    } // namespace url
} // namespace parser

