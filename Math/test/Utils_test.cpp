#include "CppUnitTest.h"

#include <Utils.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace Dubious::Math;

namespace Math_test {

TEST_CLASS(Utils_test)
{
public:

    TEST_METHOD(equals_test)
    {
        Assert::IsTrue( equals( 1, 1 ) );
        Assert::IsTrue( equals( 1, 1.0000000001f ) );
        Assert::IsTrue( equals( -1, -1 ) );
        Assert::IsTrue( equals( -1, -0.999999999f ) );
        Assert::IsTrue( equals( 0, 0 ) );
        Assert::IsTrue( equals( 0, 0.000000001f ) );
        Assert::IsTrue( equals( 0, -0.000000001f ) );
        Assert::IsTrue( equals( 0.000000001f, -0.000000001f ) );

        Assert::IsFalse( equals( 1, 1.1f ) );
        Assert::IsFalse( equals( 0, 0.1f ) );
        Assert::IsFalse( equals( 0.1f, -0.1f ) );
        Assert::IsFalse( equals( 0, -0.1f ) );
        Assert::IsFalse( equals( -1, -1.1f ) );
        Assert::IsFalse( equals( 1, -1 ) );
    }

    TEST_METHOD(radians_degrees)
    {
        Assert::IsTrue( equals( to_radians( 30 ), 0.523599f ) );
        Assert::IsTrue( equals( to_radians( 45 ), 0.785398f ) );
        Assert::IsTrue( equals( to_radians( 60 ), 1.047198f ) );
        Assert::IsTrue( equals( to_radians( 90 ), 1.570796f ) );
        Assert::IsTrue( equals( to_radians( 120 ), 2.094395f ) );
        Assert::IsTrue( equals( to_radians( 180 ), 3.141593f ) );
        Assert::IsTrue( equals( to_radians( 270 ), 4.712389f ) );

        Assert::IsTrue( equals( to_degrees( 0.52359878f ), 30 ) );
        Assert::IsTrue( equals( to_degrees( 0.7853982f ), 45 ) );
        Assert::IsTrue( equals( to_degrees( 1.0471976f ), 60 ) );
        Assert::IsTrue( equals( to_degrees( 1.57079635f ), 90 ) );
        Assert::IsTrue( equals( to_degrees( 2.0943952f ), 120 ) );
        Assert::IsTrue( equals( to_degrees( 3.1415926535f ), 180 ) );
        Assert::IsTrue( equals( to_degrees( 4.71238899f ), 270 ) );

    }

};

}