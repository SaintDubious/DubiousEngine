#include "CppUnitTest.h"

#include <Timer.h>

#include <thread>
#include <chrono>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace Dubious::Utility;

namespace TestUtility {

TEST_CLASS(Timer_test) {
public:

    TEST_METHOD(timer_tests)
    {
        Timer t;
        Assert::IsTrue( t.elapsed() < 100 );
        
        using namespace std::literals;
        std::this_thread::sleep_for(200ms);
        Assert::IsTrue( t.elapsed() < 1000 );
        Assert::IsTrue( t.elapsed() > 199 );

        Assert::IsTrue( t.restart() > 199 );
        Assert::IsTrue( t.elapsed() < 100 );
        std::this_thread::sleep_for(200ms);
        Assert::IsTrue( t.elapsed() > 199 );
    }

};
}