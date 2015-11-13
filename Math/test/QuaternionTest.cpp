#include "CppUnitTest.h"

#include <Quaternion.h>
#include <UnitVector.h>
#include <Utils.h>
#include <Triple.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using Dubious::Math::Quaternion;
using Dubious::Math::Equals;
using Dubious::Math::ToRadians;
using Dubious::Math::Triple;
using Dubious::Math::UnitVector;

namespace MathTest {

TEST_CLASS(QuaternionTest)
{
public:

    TEST_METHOD(QuaternionConstruction)
    {
        Quaternion Q;
        Assert::IsTrue( Equals(Q.m_Real, 1) );
        Assert::IsTrue( Q.m_Imaginary == Triple( 0, 0, 0 ) );

        Quaternion R( UnitVector( 1, 0, 0 ), ToRadians( 60 ) );
        Assert::IsTrue( Equals(R.m_Real, cosf(ToRadians(60)/2.0f)) );
        float HalfSin = sinf( ToRadians(60) / 2.0f );
        Assert::IsTrue( R.m_Imaginary == Triple( HalfSin, 0, 0 ) );

        Quaternion S(R);
        Assert::IsTrue( Equals(S.m_Real, cosf(ToRadians(60)/2.0f)) );
        Assert::IsTrue( S.m_Imaginary == Triple( HalfSin, 0, 0 ) );

        Q = S;
        Assert::IsTrue( Equals(Q.m_Real, cosf(ToRadians(60)/2.0f)) );
        Assert::IsTrue( Q.m_Imaginary == Triple( HalfSin, 0, 0 ) );
    }

    TEST_METHOD(QuaternionOperators)
    {
        Quaternion R( UnitVector( 0, 1, 0 ), ToRadians( 60 ) );
        Quaternion Q;
        Q = R;
        Assert::IsTrue( Equals(Q.m_Real, cosf(ToRadians(60)/2.0f)) );
        float HalfSin = sinf( ToRadians(60) / 2.0f );
        Assert::IsTrue( Q.m_Imaginary == Triple( 0, HalfSin, 0 ) );
    }

    TEST_METHOD(QuaternionLength)
    {
        // My understanding of Quaternion construction from Axis and
        // angle is that it results in a unit quaternion. This just
        // checks to see if that holds true for a bunch of samples
        for (int i=-10; i<50; ++i) {
            for (int j=-360; j<=360; j+=10) {
                Quaternion R( UnitVector( 25-i, i, i-10 ), ToRadians( j ) );
                float MagnitudeSq = R.m_Real*R.m_Real + 
                    R.m_Imaginary.m_X*R.m_Imaginary.m_X + 
                    R.m_Imaginary.m_Y*R.m_Imaginary.m_Y + 
                    R.m_Imaginary.m_Z*R.m_Imaginary.m_Z;
                Assert::IsTrue( Equals(MagnitudeSq, 1.0f) );
            }
        }
    }
};

}
