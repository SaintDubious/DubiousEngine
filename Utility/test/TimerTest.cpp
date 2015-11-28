#include "CppUnitTest.h"

#include <Timer.h>

#include <thread>
#include <chrono>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using Dubious::Utility::Timer;

namespace TestUtility
{		
TEST_CLASS(TimerTest)
{
public:

    TEST_METHOD(TimerTests)
    {
        Timer T;
        Assert::IsTrue( T.Elapsed() < 100 );
        
        using namespace std::literals;
        std::this_thread::sleep_for(200ms);
        Assert::IsTrue( T.Elapsed() < 1000 );
        Assert::IsTrue( T.Elapsed() > 199 );

        Assert::IsTrue( T.Restart() > 199 );
        Assert::IsTrue( T.Elapsed() < 100 );
        std::this_thread::sleep_for(200ms);
        Assert::IsTrue( T.Elapsed() > 199 );
    }

};
}