#include "CppUnitTest.h"

#include <CoordinateSpace.h>
#include <Point.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using Dubious::Math::CoordinateSpace;
using Dubious::Math::Point;
using Dubious::Math::Equals;

namespace MathTest {

TEST_CLASS(CoordinateSpaceTest)
{
public:

    TEST_METHOD(CoordinateSpaceConstruction)
    {
        CoordinateSpace C;
        Assert::IsTrue( C.Position() == Point( 0, 0, 0 ) );
    }

};

}