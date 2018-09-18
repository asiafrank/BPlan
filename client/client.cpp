/*=================================
读取 torrent 文件内容
https://zh.wikibooks.org/wiki/BitTorrent%E5%8D%8F%E8%AE%AE%E8%A7%84%E8%8C%83
===================================*/


#include "stdafx.h"
#include <asio.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <random>

#include "parser.h"
#include "bdecode.h"
#include "TorrentInfo.h"
#include "hash.h"
#include "peer_id.h"

using namespace std;
using namespace bplan;
namespace fs = std::filesystem;

int main()
{
    try {
        globalLog(); // init log
    }
    catch (const exception& e)
    {
        cerr << "exception: " << e.what() << endl;
        return 0;
    }
    return 0;
}