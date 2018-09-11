/*=================================
读取 torrent 文件内容
https://zh.wikibooks.org/wiki/BitTorrent%E5%8D%8F%E8%AE%AE%E8%A7%84%E8%8C%83
===================================*/


#include "stdafx.h"
#include <asio.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "bdecode.h"

using namespace std;
namespace fs = std::filesystem;

wstring readFileAndOutput(wstring fileName);

int main()
{
    wcout << "Current path is " << fs::current_path() << endl;

    // test bdecode
    try {
        bdecode(L"d1:ad1:bi1e1:c4:abcde1:di3ee");
    }
    catch (const exception& e)
    {
        wcout << L"exception: " << e.what();
    }

    // test file
    wstring fileName = L"../test_file/sample.torrent";
    wstring src = readFileAndOutput(fileName);
    try {
        bdecode(src);
    }
    catch (const exception& e)
    {
        wcout << L"exception: " << e.what();
    }

    // TODO: 1.Torrent 完整解析
    // TODO: 2.增加单元测试模块
    return 0;
}

wstring readFileAndOutput(wstring fileName)
{
    wifstream file{ fileName, ios::binary };
    if (!file)
    {
        wcout << "File opening failed" << endl;
        return L"";
    }

    file >> std::noskipws; // no skip whitespace
    wostringstream wss;
    for (wchar_t n; file >> n;) {
        wcout << n;
        wss << n;
    }
    wcout << endl;

    if (file.bad())
        std::cout << "I/O error while reading\n";
    else if (file.eof())
        std::cout << "End of file reached successfully\n";
    else if (file.fail())
        std::cout << "Non-integer data encountered\n";
    return wss.str();
}
