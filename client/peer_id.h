#pragma once

#include <string>
#include <random>
#include <sstream>

/*
20 bytes 字符串，使用 Azureus-style 格式
-<client id><version number>-<random number>

client id: "BN" 代表 BPlan
version number: 0001
*/
std::string peer_id()
{
    static std::string id;

    if (!id.empty())
        return id;

    std::ostringstream oss;
    oss << "-BN0001-";

    std::mt19937_64 prng;
    unsigned int seed = std::random_device{}();
    prng.seed(seed);
    std::uint64_t min = 100000000000;
    std::uint64_t max = 999999999999;
    std::uint64_t random = prng();
    random = random % (max - min) + min;

    oss << random;
    id = oss.str();
    return id;
}