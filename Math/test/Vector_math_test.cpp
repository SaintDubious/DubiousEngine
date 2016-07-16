#include "CppUnitTest.h"

#include <Vector_math.h>
#include <Utils.h>
#include <Unit_vector.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace Dubious::Math;

namespace Math_test {

TEST_CLASS(Vector_math_test)
{
public:

    TEST_METHOD(cross_product_test)
    {
        // XY Plane tests
        cross_test( Vector( 1, 0, 0 ), Vector( 0, 1, 0 ), Unit_vector( 0, 0, 1 ), 90 );
        cross_test( Vector( 0, 1, 0 ), Vector( 1, 0, 0 ), Unit_vector( 0, 0, -1 ), 90 );
        cross_test( Vector( 1, 1, 0 ), Vector( -1, 1, 0 ), Unit_vector( 0, 0, 1 ), 90 );
        cross_test( Vector( 1, 1, 0 ), Vector( 0, 1, 0 ), Unit_vector( 0, 0, 1 ), 45 );
        cross_test( Vector( -1, -1, 0 ), Vector( -1, 0, 0 ), Unit_vector( 0, 0, -1 ), 45 );
        cross_test( Vector( -1.732050807f, -1, 0 ), Vector( -1, -1.732050807f, 0 ), Unit_vector( 0, 0, 1 ), 30 );
        cross_test( Vector( 0, -1, 0 ), Vector( 1.732050807f, -1, 0 ), Unit_vector( 0, 0, 1 ), 60 );
        // XZ Plane tests
        cross_test( Vector( 1, 0, 1 ), Vector( -1, 0, 1 ), Unit_vector( 0, -1, 0 ), 90 );
        cross_test( Vector( 1, 0, 1 ), Vector( 0, 0, 1 ), Unit_vector( 0, -1, 0 ), 45 );
        cross_test( Vector( -1, 0, -1 ), Vector( -1, 0, 0 ), Unit_vector( 0, 1, 0 ), 45 );
        cross_test( Vector( -1.732050807f, 0, -1 ), Vector( -1, 0, -1.732050807f ), Unit_vector( 0, -1, 0 ), 30 );
        // YZ Plane tests
        cross_test( Vector( 0, 1, 1 ), Vector( 0, -1, 1 ), Unit_vector( 1, 0, 0 ), 90 );
        cross_test( Vector( 0, 1, 1 ), Vector( 0, 0, 1 ), Unit_vector( 1, 0, 0 ), 45 );
        cross_test( Vector( 0, -1, -1 ), Vector( 0, -1, 0 ), Unit_vector( -1, 0, 0 ), 45 );
        cross_test( Vector( 0, -1.732050807f, -1 ), Vector( 0, -1, -1.732050807f ), Unit_vector( 1, 0, 0 ), 30 );
        // Off Plane tests
        cross_test( Vector( 1, 1, 1 ), Vector( -1, 1, 1 ), Unit_vector( 0, -1, 1 ), 70.5287768017f );
    }

    TEST_METHOD(dot_product_test)
    {
        // XY Plane tests
        dot_test( Vector( 1, 0, 0 ), Vector( 0, 1, 0 ), 90 );
        dot_test( Vector( 0, 1, 0 ), Vector( 1, 0, 0 ), 90 );
        dot_test( Vector( 1, 1, 0 ), Vector( -1, 1, 0 ), 90 );
        dot_test( Vector( 1, 1, 0 ), Vector( 0, 1, 0 ), 45 );
        dot_test( Vector( -1, -1, 0 ), Vector( -1, 0, 0 ), 45 );
        dot_test( Vector( -1.732050807f, -1, 0 ), Vector( -1, -1.732050807f, 0 ), 30 );
        dot_test( Vector( 0, -1, 0 ), Vector( 1.732050807f, -1, 0 ), 60 );
        // XZ Plane tests
        dot_test( Vector( 1, 0, 1 ), Vector( -1, 0, 1 ), 90 );
        dot_test( Vector( 1, 0, 1 ), Vector( 0, 0, 1 ), 45 );
        dot_test( Vector( -1, 0, -1 ), Vector( -1, 0, 0 ), 45 );
        dot_test( Vector( -1.732050807f, 0, -1 ), Vector( -1, 0, -1.732050807f ), 30 );
        // YZ Plane tests
        dot_test( Vector( 0, 1, 1 ), Vector( 0, -1, 1 ), 90 );
        dot_test( Vector( 0, 1, 1 ), Vector( 0, 0, 1 ), 45 );
        dot_test( Vector( 0, -1, -1 ), Vector( 0, -1, 0 ), 45 );
        dot_test( Vector( 0, -1.732050807f, -1 ), Vector( 0, -1, -1.732050807f ), 30 );
        // Off Plane tests
        dot_test( Vector( 1, 1, 1 ), Vector( -1, 1, 1 ), 70.5287768017f );
    }

    TEST_METHOD(distance_to_line_segment_test)
    {
        Point a( 1, 1, 1 );
        Point b( 3, 3, 3 );

        Assert::IsTrue( distance_to_line_segment( a, b, Point( -1, -1, -1 ) ) == sqrt(12.0f) );
        Assert::IsTrue( distance_to_line_segment( a, b, Point( 4, 4, 4 ) ) == sqrt(3.0f) );
        Assert::IsTrue( distance_to_line_segment( a, b, Point( 2, 2, 2 ) ) == 0.0f );
        // from observation, I didn't prove this to myself. Looks like closest point is
        // (1.66666, 1.666666, 1.66666)
        Assert::IsTrue( distance_to_line_segment( a, b, Point( 2, 2, 1 ) ) == 0.816496551f );
    }

private:

    void cross_test( const Vector& v1, const Vector& v2, const Unit_vector& dir, float angle )
    {
        Vector result = cross_product( v1, v2 );
        float result_length = result.length();
        float v1_length = v1.length();
        float v2_length = v2.length();
        float sin_angle = sinf(to_radians(angle));
        float expected_length = v1_length*v2_length*sin_angle;
        Assert::IsTrue( equals( result_length, expected_length ) );
        Unit_vector result_dir( result );
        Assert::IsTrue( dir == result_dir );
    }

    void dot_test( const Vector& v1, const Vector& v2, float angle )
    {
        float result = dot_product( v1, v2 );
        float v1_length = v1.length();
        float v2_length = v2.length();
        float cos_angle = cosf(to_radians(angle));
        Assert::IsTrue( equals( result, (v1_length*v2_length*cos_angle)) );
    }
};

}