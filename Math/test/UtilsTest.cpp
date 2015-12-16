#include "CppUnitTest.h"

#include <Utils.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace Dubious::Math;

namespace MathTest {

TEST_CLASS(UtilsTest)
{
public:

    TEST_METHOD(EqualsTest)
    {
        Assert::IsTrue( Equals( 1, 1 ) );
        Assert::IsTrue( Equals( 1, 1.0000000001f ) );
        Assert::IsTrue( Equals( -1, -1 ) );
        Assert::IsTrue( Equals( -1, -0.999999999f ) );
        Assert::IsTrue( Equals( 0, 0 ) );
        Assert::IsTrue( Equals( 0, 0.000000001f ) );
        Assert::IsTrue( Equals( 0, -0.000000001f ) );
        Assert::IsTrue( Equals( 0.000000001f, -0.000000001f ) );

        Assert::IsFalse( Equals( 1, 1.1f ) );
        Assert::IsFalse( Equals( 0, 0.1f ) );
        Assert::IsFalse( Equals( 0.1f, -0.1f ) );
        Assert::IsFalse( Equals( 0, -0.1f ) );
        Assert::IsFalse( Equals( -1, -1.1f ) );
        Assert::IsFalse( Equals( 1, -1 ) );
    }

    TEST_METHOD(RadiansDegrees)
    {
        Assert::IsTrue( Equals( ToRadians( 30 ), 0.523599f ) );
        Assert::IsTrue( Equals( ToRadians( 45 ), 0.785398f ) );
        Assert::IsTrue( Equals( ToRadians( 60 ), 1.047198f ) );
        Assert::IsTrue( Equals( ToRadians( 90 ), 1.570796f ) );
        Assert::IsTrue( Equals( ToRadians( 120 ), 2.094395f ) );
        Assert::IsTrue( Equals( ToRadians( 180 ), 3.141593f ) );
        Assert::IsTrue( Equals( ToRadians( 270 ), 4.712389f ) );

        Assert::IsTrue( Equals( ToDegrees( 0.52359878f ), 30 ) );
        Assert::IsTrue( Equals( ToDegrees( 0.7853982f ), 45 ) );
        Assert::IsTrue( Equals( ToDegrees( 1.0471976f ), 60 ) );
        Assert::IsTrue( Equals( ToDegrees( 1.57079635f ), 90 ) );
        Assert::IsTrue( Equals( ToDegrees( 2.0943952f ), 120 ) );
        Assert::IsTrue( Equals( ToDegrees( 3.1415926535f ), 180 ) );
        Assert::IsTrue( Equals( ToDegrees( 4.71238899f ), 270 ) );

    }

};

}