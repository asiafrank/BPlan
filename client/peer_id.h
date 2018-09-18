#pragma once

#include <string>
#include <random>
#include <sstream>

/*
20 bytes �ַ�����ʹ�� Azureus-style ��ʽ
-<client id><version number>-<random number>

client id: "BN" ���� BPlan
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