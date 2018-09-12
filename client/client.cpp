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

string readFileAndOutput(string fileName);

int main()
{
    cout << "Current path is " << fs::current_path() << endl;

    // test bdecode
    try {
        bdecode("d1:ad1:bi1e1:c4:abcde1:di3ee");
    }
    catch (const exception& e)
    {
        cout << "exception: " << e.what();
    }

    // test file
    string fileName = "../test_file/sample.torrent";
    string src = readFileAndOutput(fileName);
    try {
        bdecode(src);
    }
    catch (const exception& e)
    {
        cout << "exception: " << e.what();
    }

    // TODO: 1.Torrent 完整解析
    // TODO: 2.增加单元测试模块
    return 0;
}

string readFileAndOutput(string fileName)
{
    ifstream file{ fileName, ios::binary };
    if (!file)
    {
        cout << "File opening failed" << endl;
        return "";
    }

    file >> std::noskipws; // no skip whitespace
    ostringstream ss;
    for (char n; file >> n;) {
        cout << n;
        ss << n;
    }
    cout << endl;

    if (file.bad())
        cout << "I/O error while reading\n";
    else if (file.eof())
        cout << "End of file reached successfully\n";
    else if (file.fail())
        cout << "Non-integer data encountered\n";
    return ss.str();
}
