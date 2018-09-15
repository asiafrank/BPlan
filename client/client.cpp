/*=================================
读取 torrent 文件内容
https://zh.wikibooks.org/wiki/BitTorrent%E5%8D%8F%E8%AE%AE%E8%A7%84%E8%8C%83
===================================*/


#include "stdafx.h"
#include <asio.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>

#include "parser.h"
#include "bdecode.h"
#include "TorrentInfo.h"

using namespace std;
namespace fs = std::filesystem;

void stdout_example();

int main()
{
    try {
        globalLog(); // init log
    }
    catch (const exception& e)
    {
        cout << "exception: " << e.what() << endl;
        return 0;
    }

    auto logger = globalLog();
    logger->info("Current path is {}", nowide::narrow(fs::current_path().c_str()));

    // test file
    //const string fileName("../test_file/sample.torrent");
    const string fileName("../test_file/cce6bb1700bf8b2396cded991b34fb2bf4107b25.torrent");

    /*shared_ptr<TorrentInfo> pInfo = makeTorrentInfo(fileName);
    if (pInfo)
        logger->info("{}", pInfo->str());
    else
        logger->info("analyse failed");*/

    stdout_example();

    try {
        auto console = spdlog::get("console");
        // string 中存储的编码形式时系统的编码形式，windows下默认时 GBK，需要手动指定：chcp 65001
        // 或者程序指定
        /*if (!SetConsoleOutputCP(65001)) {
            console->error("SetConsoleOutputCP failed!");
        }*/
    }
    catch (const exception& e)
    {
        cout << "exception: " << e.what() << endl;
    }
    return 0;
}

// console log example
#include <spdlog/sinks/stdout_color_sinks.h>

void stdout_example()
{
    // create color multi threaded logger
    auto console = spdlog::stdout_color_mt("console");
    console->info("Welcome to spdlog!");
    console->error("Some error message with arg: {}", 1);

    auto err_logger = spdlog::stderr_color_mt("stderr");
    err_logger->error("Some error message");

    // Formatting examples
    console->warn("Easy padding in numbers like {:08d}", 12);
    console->critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
    console->info("Support for floats {:03.2f}", 1.23456);
    console->info("Positional args are {1} {0}..", "too", "supported");
    console->info("{:<30}", "left aligned");

    spdlog::get("console")->info("loggers can be retrieved from a global registry using the spdlog::get(logger_name)");

    // Runtime log levels
    spdlog::set_level(spdlog::level::info); // Set global log level to info
    console->debug("This message should not be displayed!");
    console->set_level(spdlog::level::trace); // Set specific logger's log level
    console->debug("This message should be displayed..");

    // Customize msg format for all loggers
    spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
    console->info("This an info message with custom format");

    // Compile time log levels
    // define SPDLOG_DEBUG_ON or SPDLOG_TRACE_ON
    SPDLOG_TRACE(console, "Enabled only #ifdef SPDLOG_TRACE_ON..{} ,{}", 1, 3.23);
    SPDLOG_DEBUG(console, "Enabled only #ifdef SPDLOG_DEBUG_ON.. {} ,{}", 1, 3.23);
}