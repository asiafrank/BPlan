#include "stdafx.h"
#include "CppUnitTest.h"
#include "../client/bdecode.h"
#include "../client/TorrentInfo.h"
#include "../client/parser.h"

/*
add library
https://docs.microsoft.com/en-us/visualstudio/test/unit-testing-existing-cpp-applications-with-test-explorer?view=vs-2015

test api
https://docs.microsoft.com/en-us/visualstudio/test/microsoft-visualstudio-testtools-cppunittestframework-api-reference?view=vs-2017
*/

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test
{
    TEST_CLASS(BDecodeHTest)
    {
    public:
        TEST_METHOD(TestBdecode)
        {
            std::shared_ptr<Node> proot = bdecode("d1:ad1:bi1e1:c4:abcde1:di3ee");
            const std::string& rootValue = proot->getValue();
            const std::string& expect = "{\"a\":{\"b\":1,\"c\":\"abcd\"},\"d\":3}";
            Assert::AreEqual(expect, rootValue);
        }
    };

    TEST_CLASS(TorrentInfo)
    {
    public:
        TEST_METHOD(TestToReadableHex)
        {
            std::ostringstream oss;
            oss << readable_base16_high('h') << readable_base16_low('h');
            oss << readable_base16_high('l') << readable_base16_low('l');
            Assert::AreEqual(std::string("686C"), oss.str());
        }
    };

    TEST_CLASS(Parser)
    {
    public:
        TEST_METHOD(TestUTF8)
        {
            // 指定 string 以 utf8 存储
            string u8str = u8"http:://www.abc.com?keyword=中文";
            string u8str_encoded = parser::url::encode(u8str);

            // 系统默认编码存储
            string codepage_str = "http:://www.abc.com?keyword=中文";
            string codepage_str_encoded = parser::url::encode(codepage_str);
            Assert::AreEqual(u8str_encoded, codepage_str_encoded);
        }
    };
}