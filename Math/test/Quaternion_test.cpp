#include "CppUnitTest.h"

#include <Quaternion.h>
#include <Unit_vector.h>
#include <Utils.h>
#include <Triple.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace Dubious::Math;

namespace Math_test {

TEST_CLASS(Quaternion_test)
{
public:

    TEST_METHOD(quaternion_construction)
    {
        Quaternion q;
        Assert::IsTrue( equals(q.m_real, 1) );
        Assert::IsTrue( q.m_imaginary == Triple( 0, 0, 0 ) );
        Assert::IsTrue( equals( q.magnitude(), 1) );

        Quaternion r( Unit_vector( 1, 0, 0 ), to_radians( 60 ) );
        Assert::IsTrue( equals(r.m_real, cosf(to_radians(60)/2.0f)) );
        float HalfSin = sinf( to_radians(60) / 2.0f );
        Assert::IsTrue( r.m_imaginary == Triple( HalfSin, 0, 0 ) );
        Assert::IsTrue( equals( r.magnitude(), 1) );

        Quaternion s(r);
        Assert::IsTrue( equals(s.m_real, cosf(to_radians(60)/2.0f)) );
        Assert::IsTrue( s.m_imaginary == Triple( HalfSin, 0, 0 ) );
        Assert::IsTrue( equals( s.magnitude(), 1) );

        q = s;
        Assert::IsTrue( equals(q.m_real, cosf(to_radians(60)/2.0f)) );
        Assert::IsTrue( q.m_imaginary == Triple( HalfSin, 0, 0 ) );
        Assert::IsTrue( equals( q.magnitude(), 1) );
}

    TEST_METHOD(quaternion_operators)
    {
        Quaternion r( Unit_vector( 0, 1, 0 ), to_radians( 60 ) );
        Quaternion q;
        Assert::IsTrue( equals( q.magnitude(), 1) );
        Assert::IsTrue( equals( r.magnitude(), 1) );

        Assert::IsTrue( q != r );
        q = r;
        Assert::IsTrue( q == r );
        Assert::IsTrue( equals(q.m_real, cosf(to_radians(60)/2.0f)) );
        float half_sin = sinf( to_radians(60) / 2.0f );
        Assert::IsTrue( q.m_imaginary == Triple( 0, half_sin, 0 ) );

        Quaternion s( Unit_vector( 0, 1, 0), to_radians( 20 ) );
        Assert::IsTrue( equals( s.magnitude(), 1) );
        Quaternion t = s*r;
        Assert::IsTrue( equals( t.magnitude(), 1) );
        Quaternion u( Unit_vector( 0, 1, 0), to_radians( 80 ) );
        Assert::IsTrue( t == u );

        s = Quaternion( Unit_vector( 0, 1, 0 ), to_radians( 10 ) );
        t = Quaternion( Unit_vector( 0, 1, 0 ), to_radians( 10 ) );
        for (int i=2; i<18; ++i) {
            s = t*s;
            u = Quaternion( Unit_vector( 0, 1, 0 ), to_radians(i*10.0f) );
            Assert::IsTrue( equals( u.magnitude(), 1) );
            Assert::IsTrue( s == u );
            Quaternion conjugate = s.conjugate();
            Assert::IsTrue( equals( conjugate.magnitude(), 1) );
            Assert::IsTrue( conjugate.m_real == s.m_real );
            Assert::IsTrue( conjugate.m_imaginary.m_x == -s.m_imaginary.m_x );
            Assert::IsTrue( conjugate.m_imaginary.m_y == -s.m_imaginary.m_y );
            Assert::IsTrue( conjugate.m_imaginary.m_z == -s.m_imaginary.m_z );
        }
        s = Quaternion( Unit_vector( 0, 1, 0 ), to_radians( 10 ) );
        t = Quaternion( Unit_vector( 0, 1, 0 ), to_radians( 10 ) );
        u = s + t;
        Assert::IsFalse( equals( u.magnitude(), 1) );
        u.normalize();
        Assert::IsTrue( equals( u.magnitude(), 1) );
        u = s * 10;
        Assert::IsFalse( equals( u.magnitude(), 1) );
        u.normalize();
        Assert::IsTrue( equals( u.magnitude(), 1) );
    }

    TEST_METHOD(quaternion_length)
    {
        // My understanding of Quaternion construction from Axis and
        // angle is that it results in a unit quaternion. This just
        // checks to see if that holds true for a bunch of samples
        for (int i=-10; i<50; ++i) {
            for (int j=-360; j<=360; j+=10) {
                Quaternion r( Unit_vector( static_cast<float>(25-i), static_cast<float>(i), static_cast<float>(i-10) ), 
                              to_radians( static_cast<float>(j) ) );
                Assert::IsTrue( equals( r.magnitude(), 1) );
            }
        }
    }

    TEST_METHOD(quaternion_matrix)
    {
        float matrix[16];
        Quaternion a;
        a.get_matrix( matrix );

        using namespace Dubious::Math::Matrix_index;

        Assert::IsTrue( equals( matrix[_11], 1 ) );
        Assert::IsTrue( equals( matrix[_22], 1 ) );
        Assert::IsTrue( equals( matrix[_33], 1 ) );

        Quaternion b( Unit_vector(0, 1, 0), to_radians(30) );
        b.get_matrix( matrix );
        // X Axis
        Assert::IsTrue( equals( matrix[_11], cosf(to_radians(30)) ) );
        Assert::IsTrue( equals( matrix[_21], 0 ) );
        Assert::IsTrue( equals( matrix[_31], -sinf(to_radians(30)) ) );
        // Y Axis
        Assert::IsTrue( equals( matrix[_12], 0 ) );
        Assert::IsTrue( equals( matrix[_22], 1 ) );
        Assert::IsTrue( equals( matrix[_32], 0 ) );
        // Z Axis
        Assert::IsTrue( equals( matrix[_13], sinf(to_radians(30)) ) );
        Assert::IsTrue( equals( matrix[_23], 0 ) );
        Assert::IsTrue( equals( matrix[_33], cosf(to_radians(30)) ) );
    }

    TEST_METHOD(quaternion_axis_angle)
    {
        Unit_vector axis( 0.5f, 1.7f, -1.1f );
        float angle = 1.2345f;
        Quaternion a( axis, angle );
        Unit_vector test_axis;
        float test_angle;
        std::tie(test_axis, test_angle) = a.get_axis_angle();

        Assert::IsTrue( axis == test_axis );
        Assert::IsTrue( angle == test_angle );
    }
};

}
