/*=================================
读取 torrent 文件内容
https://zh.wikibooks.org/wiki/BitTorrent%E5%8D%8F%E8%AE%AE%E8%A7%84%E8%8C%83
===================================*/


#include "stdafx.h"
#include <asio.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <nowide/args.hpp>
#include <nowide/fstream.hpp>
#include <nowide/iostream.hpp>

#include "bdecode.h"
#include "TorrentInfo.h"

using namespace std;
namespace fs = std::filesystem;

int main()
{
    cout << "Current path is " << fs::current_path() << endl;

    // test bdecode
    try {
        shared_ptr<Node> proot = bdecode("d1:ad1:bi1e1:c4:abcde1:di3ee");
        cout << proot->getValue() << endl;
        std::string expect = "{\"a\":{\"b\":1,\"c\":\"abcd\"},\"d\":3}";

        cout << "equal : " << (proot->getValue() == expect) << endl;
    }
    catch (const exception& e)
    {
        cout << "exception: " << e.what() << endl;
    }

    // test file
    //const string fileName("../test_file/sample.torrent");
    const string fileName("../test_file/cce6bb1700bf8b2396cded991b34fb2bf4107b25.torrent");

    shared_ptr<TorrentInfo> pInfo = makeTorrentInfo(fileName);
    if (pInfo)
        nowide::cout << pInfo->str() << endl;
    else
        nowide::cout << "analyse failed" << endl;
    // TODO: 1.增加单元测试模块
    return 0;
}
