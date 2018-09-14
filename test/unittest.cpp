#include "stdafx.h"
#include "CppUnitTest.h"
#include "../client/bdecode.h"
#include "../client/TorrentInfo.h"

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
}