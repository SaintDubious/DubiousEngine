#include "CppUnitTest.h"

#include <Quaternion.h>
#include <Utils.h>
#include <Triple.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using Dubious::Math::Quaternion;
using Dubious::Math::Equals;
using Dubious::Math::Triple;

namespace MathTest {

TEST_CLASS(QuaternionTest)
{
public:

    TEST_METHOD(QuaternionConstruction)
    {
        Quaternion Q;
        Assert::IsTrue( Equals(Q.m_Real, 1) );
        Assert::IsTrue( Q.m_Imaginary == Triple( 0, 0, 0 ) );
    }

};

}
