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

        Quaternion S( UnitVector( 0, 1, 0), ToRadians( 20 ) );
        Quaternion T = S*R;
        Quaternion U( UnitVector( 0, 1, 0), ToRadians( 80 ) );
        Assert::IsTrue( T == U );

        S = Quaternion( UnitVector( 0, 1, 0 ), ToRadians( 10 ) );
        T = Quaternion( UnitVector( 0, 1, 0 ), ToRadians( 10 ) );
        for (int i=2; i<18; ++i) {
            S = T*S;
            U = Quaternion( UnitVector( 0, 1, 0 ), ToRadians(i*10) );
            Assert::IsTrue( S == U );
        }
    }

    TEST_METHOD(QuaternionLength)
    {
        // My understanding of Quaternion construction from Axis and
        // angle is that it results in a unit quaternion. This just
        // checks to see if that holds true for a bunch of samples
        for (int i=-10; i<50; ++i) {
            for (int j=-360; j<=360; j+=10) {
                Quaternion R( UnitVector( static_cast<float>(25-i), static_cast<float>(i), static_cast<float>(i-10) ), 
                              ToRadians( static_cast<float>(j) ) );
                Assert::IsTrue( Equals(MagnitudeSq(R), 1.0f) );
            }
        }
    }

    TEST_METHOD(QuaternionMatrix)
    {
        float Matrix[16];
        Quaternion A;
        A.GetMatrix( Matrix );

        using namespace Dubious::Math::MatrixIndex;

        Assert::IsTrue( Equals( Matrix[_11], 1 ) );
        Assert::IsTrue( Equals( Matrix[_22], 1 ) );
        Assert::IsTrue( Equals( Matrix[_33], 1 ) );

        Quaternion B( UnitVector(0, 1, 0), ToRadians(30) );
        B.GetMatrix( Matrix );
        // X Axis
        Assert::IsTrue( Equals( Matrix[_11], cosf(ToRadians(30)) ) );
        Assert::IsTrue( Equals( Matrix[_21], 0 ) );
        Assert::IsTrue( Equals( Matrix[_31], -sinf(ToRadians(30)) ) );
        // Y Axis
        Assert::IsTrue( Equals( Matrix[_12], 0 ) );
        Assert::IsTrue( Equals( Matrix[_22], 1 ) );
        Assert::IsTrue( Equals( Matrix[_32], 0 ) );
        // Z Axis
        Assert::IsTrue( Equals( Matrix[_13], sinf(ToRadians(30)) ) );
        Assert::IsTrue( Equals( Matrix[_23], 0 ) );
        Assert::IsTrue( Equals( Matrix[_33], cosf(ToRadians(30)) ) );
    }

private:
    float MagnitudeSq( const Quaternion& A ) const
    {
        return A.m_Real*A.m_Real + 
            A.m_Imaginary.m_X*A.m_Imaginary.m_X + 
            A.m_Imaginary.m_Y*A.m_Imaginary.m_Y + 
            A.m_Imaginary.m_Z*A.m_Imaginary.m_Z;
    }
};

}
