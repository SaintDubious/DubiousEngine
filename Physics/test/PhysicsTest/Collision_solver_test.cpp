#include "CppUnitTest.h"

#include <Physics_model.h>
#include <Physics_object.h>
#include <Collision_solver.h>
#include <Triple.h>
#include <Coordinate_space.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Dubious::Physics;
using namespace Dubious::Math;

namespace Physics_test
{		
TEST_CLASS(Collision_solver_test) {
public:
		
	TEST_METHOD(collision_solver_intersection)
	{
        Collision_solver solver;
        std::shared_ptr<Physics_model> model( new Physics_model( Triple( 1, 1, 1 ) ) );
        Physics_object a( model );
        Physics_object b( model );

        b.coordinate_space().translate( Vector( 3, 0, 0 ) );
        Assert::IsTrue( solver.intersection( a, b ) == false );
        // Touching... should this be a collision?
        b.coordinate_space().translate( Vector( -1, 0, 0 ) );
        Assert::IsTrue( solver.intersection( a, b ) == false );
        // definite collision
        b.coordinate_space().translate( Vector( -1, 0, 0 ) );
        Assert::IsTrue( solver.intersection( a, b ) == true );

        // Add some rotation
        b.coordinate_space().rotate( Quaternion( Vector( 0, 1, 0 ), to_radians( 30 ) ) );
        Assert::IsTrue( solver.intersection( a, b ) == true );
        b.coordinate_space().rotate( Quaternion( Vector( 1, 0, 0 ), to_radians( 30 ) ) );
        Assert::IsTrue( solver.intersection( a, b ) == true );

        // Test that failed in CollisionViewer
        a.coordinate_space() = Coordinate_space( Point(1, 4, -1), Quaternion(-0.191323161, Triple(0.479090452, 0.852346003, 0.0858706832)) );
        b.coordinate_space() = Coordinate_space( Point(2, 2, -1), Quaternion(-0.737145424, Triple(-0.451694548, 0.245187759, 0.438716054)) );
        Assert::IsTrue( solver.intersection( a, b ) == true );
    }

};
}