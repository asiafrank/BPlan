// BPlan-server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <asio.hpp>
#include <iostream>

int main()
{
    asio::io_context io;
    asio::steady_timer t(io, asio::chrono::seconds(5));
    t.wait();
    std::cout << "Hello, world!" << std::endl;

    // TODO: implement bittorrent
    return 0;
}

