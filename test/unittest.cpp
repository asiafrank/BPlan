#include "stdafx.h"
#include "CppUnitTest.h"

#include <filesystem>
#include "../client/bdecode.h"
#include "../client/TorrentInfo.h"
#include "../client/parser.h"
#include "../client/hash.h"
#include "../client/hex.h"
#include "../client/peer_id.h"

/*
add library
https://docs.microsoft.com/en-us/visualstudio/test/unit-testing-existing-cpp-applications-with-test-explorer?view=vs-2015

test api
https://docs.microsoft.com/en-us/visualstudio/test/microsoft-visualstudio-testtools-cppunittestframework-api-reference?view=vs-2017
*/

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace bplan;
namespace test
{
    TEST_CLASS(BDecodeHTest)
    {
    public:
        TEST_METHOD(TestBdecode)
        {
            std::string raw = "d1:ad1:bi1e1:c4:abcde1:di3ee";
            std::shared_ptr<Node> proot = bdecode(raw);
            proot->collect();

            const std::string rootRaw = proot->getRaw();
            const std::string& rootStr = proot->str();
            const std::string& expect = "{\"a\":{\"b\":1,\"c\":\"abcd\"},\"d\":3}";
            Assert::AreEqual(expect, rootStr);
            Assert::AreEqual(raw, rootRaw);
        }
    };

    TEST_CLASS(TorrentInfo)
    {
    public:
        TEST_METHOD(TestHash)
        {
            const string input = "abc";

            SHA1 checksum;
            checksum.update(input);
            const string hash = checksum.final();
            Assert::AreEqual(string("a9993e364706816aba3e25717850c26c9cd0d89d"), hash);
        }

        TEST_METHOD(TestHex)
        {
            const string s("hl");
            string hex = to_hex(s);
            Assert::AreEqual(std::string("686c"), hex);
        }

        TEST_METHOD(TestInfoHash)
        {
            Logger::WriteMessage("test info hash start");
            const string infohash = "cce6bb1700bf8b2396cded991b34fb2bf4107b25";
            ostringstream oss;
            oss << "../../test_file/" << infohash << ".torrent";
            const string fileName(oss.str());

            shared_ptr<bplan::TorrentInfo> pInfo = makeTorrentInfo(fileName);
            if (!pInfo)
            {
                Logger::WriteMessage("file path not correct, current path is:");
                Logger::WriteMessage(std::filesystem::current_path().c_str());
                return;
            }

            oss.clear();
            oss.str("");
            oss << "test arrive hex " << pInfo->getInfoSha1Hex() << endl;
            Assert::AreEqual(infohash, pInfo->getInfoSha1Hex());
        }

        TEST_METHOD(TestPeerId)
        {
            string pid = peer_id();
            Assert::IsFalse(pid.empty());
        }
    };

    TEST_CLASS(Parser)
    {
    public:
        TEST_METHOD(TestUrlEncodeAndDecode)
        {
            // ָ�� string �� utf8 �洢
            string u8str = u8"http://www.abc.com?keyword=����";
            string u8str_encoded = url::encode(u8str);

            // ϵͳĬ�ϱ���洢
            string codepage_str = "http://www.abc.com?keyword=����";
            string codepage_str_encoded = url::encode(codepage_str);
            Assert::AreEqual(u8str_encoded, codepage_str_encoded);

            /*
            decode ����֧������
            string decoded = url::decode(codepage_str_encoded);

            Logger::WriteMessage(decoded.c_str());
            Assert::AreEqual(u8str, decoded);*/
        }
    };
}