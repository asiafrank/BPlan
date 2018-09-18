#pragma once

#include <iomanip>
#include <string>
#include <sstream>

#include <utf8.h>
#include <nowide/iostream.hpp>

#include "hex.h"

using namespace std;
namespace bplan {
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

            if (i == ' ') // 空格只需处理成 + 号
                return true;
            return false;
        }

        /*
        先将 string 转成 UTF8 存储，再进行 URL 编码
        TODO: 勉强支持 UTF8。参考 java.net.URLEncoder 以做修改
        */
        string encode(const string& s)
        {
            string u8s = s;
            string::iterator end_it = utf8::find_invalid(u8s.begin(), u8s.end());
            if (end_it != u8s.end()) { // not utf8
                u8s = toUTF8(u8s);     // convert to utf8
            }

            ostringstream encodedss;
            encodedss.fill('0');
            encodedss << hex;

            for (string::const_iterator it = u8s.begin(), end = u8s.end(); it != end; ++it) {
                string::value_type c = (*it);

                // Keep alphanumeric and other accepted characters intact
                if (dontNeedEncoding(c)) {
                    if (c == ' ')
                        c = '+';
                    encodedss << c;
                    continue;
                }

                // Any other characters are percent-encoded
                encodedss << uppercase;
                encodedss << '%' << (int)(unsigned char)c;
                encodedss << nouppercase;
            }

            return encodedss.str();
        }

        /*
        URL 解码
        TODO: 参考 java.net.URLDecoderm，以支持 UTF8
        */
        string decode(const string& s)
        {
            ostringstream decodedss;
            ostringstream buf;
            for (string::const_iterator it = s.begin(), end = s.end(); it != end; ++it) {
                string::value_type c = (*it);

                if (c != '%') {
                    if (c == '+')
                        c = ' ';
                    decodedss << c;
                    continue;
                }

                while (it != end)
                {
                    c = *it;
                    if (c != '%')
                        break;
                    // next 2 char
                    int c1 = detail::hex_to_int(*(++it));
                    int c2 = detail::hex_to_int(*(++it));
                    int x = c1 * 16 + c2;
                    buf << (char)x;

                    if (it != end)
                        ++it;
                }

                --it; // while 循环结束后，回退一格

                string ch2 = buf.str();
                decodedss << toUTF8(ch2);
                buf.clear();
                buf.str("");
            }

            return decodedss.str();
        }
    } // namespace url
} // namespace bplan

