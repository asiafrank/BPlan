#pragma once

#include <iomanip>
#include <string>
#include <sstream>

using namespace std;
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

    bool wdontNeedEncoding(wchar_t c)
    {
        int i = (int)c;
        if (i >= L'0' && i <= L'9')
            return true;

        if (i >= L'a' && i <= L'z')
            return true;

        if (i >= L'A' && i <= L'Z')
            return true;

        if (i == L'-' || i == L'_' || i == '.' || i == '~')
            return true;
        
        return false;
    }

    // TODO: 支持中文
    string encode(const string& value) 
    {
        ostringstream escaped;
        escaped.fill('0');
        escaped << hex;

        for (string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
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

    // not work
    wstring encode2(const string& value)
    {
        wstring wvalue = nowide::widen(value);
        wostringstream escaped;
        escaped.fill('0');
        escaped << hex;

        for (wstring::const_iterator i = wvalue.begin(), n = wvalue.end(); i != n; ++i) {
            wstring::value_type c = (*i);

            // Keep alphanumeric and other accepted characters intact
            if (dontNeedEncoding(c)) {
                escaped << c;
                continue;
            }

            // Any other characters are percent-encoded
            escaped << uppercase;
            escaped << '%' << (int)c;
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
}