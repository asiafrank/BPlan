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

// TODO: 编写 encode 方法。

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    // Use HeapSetInformation to specify that the process should
    // terminate if the heap manager detects an error in any heap used
    // by the process.
    // The return value is ignored, because we want to continue running in the
    // unlikely event that HeapSetInformation fails.
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

#ifdef ASIO_ENABLE_HANDLER_TRACKING
    // 将 std err 重定向到文件日志中
#ifdef _WIN32
    HANDLE h = CreateFile(L"err.log", GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, 0);
    SetStdHandle(STD_ERROR_HANDLE, h);
#else
    std::ofstream ofs("test.log", std::ios::binary);
    std::cerr.rdbuf(ofs.rdbuf());
#endif // _WIN32
#endif // ASIO_ENABLE_HANDLER_TRACKING

    // _crtBreakAlloc = 296;

    try {
        globalLog(); // init log

        // TODO：UI 界面
        MessageBox(0, L"Hello World!", 0, 0);
    }
    catch (const exception& e)
    {
        cerr << "exception: " << e.what() << endl;
        return 0;
    }

    // 关闭 err.txt 文件
#ifdef ASIO_ENABLE_HANDLER_TRACKING
#ifdef _WIN32
    CloseHandle(h);
#else
    ofs.close();
#endif // _WIN32
#endif // ASIO_ENABLE_HANDLER_TRACKING
    return 0;
}