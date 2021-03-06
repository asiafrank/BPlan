// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>


// TODO: reference additional headers your program requires here
// c++ 17 ignore warning
#define _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING
//#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS

#define ASIO_STANDALONE 
#define ASIO_HAS_STD_ADDRESSOF
#define ASIO_HAS_STD_ARRAY
#define ASIO_HAS_CSTDINT
#define ASIO_HAS_STD_SHARED_PTR
#define ASIO_HAS_STD_TYPE_TRAITS

#define NOWIDE_SOURCE

#include <nowide/args.hpp>
#include <nowide/fstream.hpp>
#include <nowide/iostream.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include "exception.h"

static std::shared_ptr<spdlog::logger> globalLog()
{
    static auto plog = spdlog::get("global_log");
    if (plog)
        return plog;
    
    plog = spdlog::rotating_logger_mt("global_log", "bplan.log", 1048576 * 5, 3);
    return plog;
}