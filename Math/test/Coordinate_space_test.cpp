#include "CppUnitTest.h"

#include <Coordinate_space.h>
#include <Point.h>

#include <tuple>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace Dubious::Math;

namespace Math_test {

TEST_CLASS(Coordinate_space_test)
{
public:

    TEST_METHOD(coordinate_space_construction)
    {
        Coordinate_space c;
        Assert::IsTrue( c.position() == Point( 0, 0, 0 ) );
    }

    TEST_METHOD(coordinate_space_operators)
    {
        Coordinate_space a,b;
        Assert::IsTrue( a == b );
        b.translate( Vector( 1, 0, 0 ) );
        Assert::IsFalse( a == b );
        b.translate( Vector( -1, 0, 0 ) );
        Assert::IsTrue( a == b );
        b.rotate( Quaternion( Unit_vector( 0, 1, 0 ), to_radians(45) ) );
        Assert::IsFalse( a == b );
        b.translate( Vector( 1, 0, 0 ) );
        Assert::IsFalse( a == b );
    }

    TEST_METHOD(coordinate_space_translation)
    {
        Coordinate_space c;
        Assert::IsTrue( c.position() == Point( 0, 0, 0 ) );
        c.translate( Vector( 1, 2, 3) );
        Assert::IsTrue( c.position() == Point( 1, 2, 3 ) );
        c.translate( Vector( 1, 2, 3) );
        Assert::IsTrue( c.position() == Point( 2, 4, 6 ) );

        c.rotate( Quaternion( Unit_vector( 0, 1, 0 ), to_radians( 90 ) ) );
        c.translate( Vector( 1, 2, 3) );
        Assert::IsTrue( c.position() == Point( 3, 6, 9 ) );
        c.translate( Local_vector( 1, 2, 3) );
        Assert::IsTrue( c.position() == Point( 6, 8, 8 ) );
    }

    TEST_METHOD(coordinate_space_rotation)
    {
        Coordinate_space c;
        Unit_vector x, y, z;
        std::tie( x, y, z ) = c.get_axes();
        Assert::IsTrue( x == Unit_vector( 1, 0, 0 ) );
        Assert::IsTrue( y == Unit_vector( 0, 1, 0 ) );
        Assert::IsTrue( z == Unit_vector( 0, 0, 1 ) );
        c.rotate( Quaternion( Unit_vector( 0, 1, 0 ), to_radians( 90 ) ) );
        std::tie( x, y, z ) = c.get_axes();
        Assert::IsTrue( x == Unit_vector( 0, 0, -1 ) );
        Assert::IsTrue( y == Unit_vector( 0, 1, 0 ) );
        Assert::IsTrue( z == Unit_vector( 1, 0, 0 ) );
        c.rotate( Quaternion( Unit_vector( 0, 0, 1 ), to_radians( 90 ) ) );
        std::tie( x, y, z ) = c.get_axes();
        Assert::IsTrue( x == Unit_vector( 0, 0, -1 ) );
        Assert::IsTrue( y == Unit_vector( -1, 0, 0 ) );
        Assert::IsTrue( z == Unit_vector( 0, 1, 0 ) );

        c.rotate( Local_quaternion( Local_unit_vector( 0, 0, -1 ), to_radians( 90 ) ) );
        std::tie( x, y, z ) = c.get_axes();
        Assert::IsTrue( x == Unit_vector( 1, 0, 0 ) );
        Assert::IsTrue( y == Unit_vector( 0, 0, -1 ) );
        Assert::IsTrue( z == Unit_vector( 0, 1, 0 ) );
        c.rotate( Local_quaternion( Local_unit_vector( 0, 1, 0 ), to_radians( 90 ) ) );
        std::tie( x, y, z ) = c.get_axes();
        Assert::IsTrue( x == Unit_vector( 0, -1, 0 ) );
        Assert::IsTrue( y == Unit_vector( 0, 0, -1 ) );
        Assert::IsTrue( z == Unit_vector( 1, 0, 0 ) );
    }

    TEST_METHOD(coordinate_space_vectortransform)
    {
        Coordinate_space c;
        c.rotate( Quaternion( Vector( 0, 1, 0 ), to_radians( 90 ) ) );
        Vector v = c.transform( Local_vector( 1, 1, 1 ) );
        Assert::IsTrue( v == Vector( 1, 1, -1 ) );
        Local_vector lv = c.transform( Vector( 1, 1, 1 ) );
        Assert::IsTrue( lv == Local_vector( -1, 1, 1 ) );
    }

    TEST_METHOD(coordinate_space_pointtransform)
    {
        Coordinate_space c;
        c.rotate( Quaternion( Vector( 0, 1, 0 ), to_radians( 90 ) ) );
        c.position() = Point( 5, 5, 5 );
        Point p = c.transform( Local_point( 1, 1, 1 ) );
        Assert::IsTrue( p == Point( 6, 6, 4 ) );
        Local_point lp = c.transform( Point( 1, 1, 1 ) );
        Assert::IsTrue( lp == Local_point( 4, -4, -4 ) );
    }

};

}