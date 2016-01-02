#include "CppUnitTest.h"

#include <PhysicsModel.h>
#include <PhysicsObject.h>
#include <CollisionSolver.h>
#include <Triple.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Dubious::Physics;
using namespace Dubious::Math;

namespace PhysicsTest
{		
TEST_CLASS(CollisionSolverTest)
{
public:
		
	TEST_METHOD(CollisionSolverIntersection)
	{
        CollisionSolver Solver;
        PhysicsModelPtr pModel( new PhysicsModel( Triple( 1, 1, 1 ) ) );
        PhysicsObject A( pModel );
        PhysicsObject B( pModel );

        B.CoordinateSpace().Translate( Vector( 3, 0, 0 ) );
        Assert::IsTrue( Solver.Intersection( A, B ) == false );
        // Touching... should this be a collision?
        B.CoordinateSpace().Translate( Vector( -1, 0, 0 ) );
        Assert::IsTrue( Solver.Intersection( A, B ) == false );
        // definite collision
        B.CoordinateSpace().Translate( Vector( -1, 0, 0 ) );
        Assert::IsTrue( Solver.Intersection( A, B ) == true );

        // Add some rotation
        B.CoordinateSpace().Rotate( Quaternion( Vector( 0, 1, 0 ), ToRadians( 30 ) ) );
        Assert::IsTrue( Solver.Intersection( A, B ) == true );
        B.CoordinateSpace().Rotate( Quaternion( Vector( 1, 0, 0 ), ToRadians( 30 ) ) );
        Assert::IsTrue( Solver.Intersection( A, B ) == true );
    }

};
}