#include "CppUnitTest.h"

#include <RotationVector.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using Dubious::Math::RotationVector;
using Dubious::Math::UnitVector;

namespace MathTest {

TEST_CLASS(RotationVectorTest)
{
public:

    TEST_METHOD(RotationVectorConstruction)
    {
        RotationVector V;
        Assert::IsTrue( V.Axis() == UnitVector( 1, 0, 0 ) );
        Assert::IsTrue( V.Angle() == 0.0f );
    }
};
}
