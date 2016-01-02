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
        B.CoordinateSpace().Translate( Vector( -2, 0, 0 ) );
        Assert::IsTrue( Solver.Intersection( A, B ) == false );
        B.CoordinateSpace().Translate( Vector( -0.5, 0.5, 0 ) );
        Assert::IsTrue( Solver.Intersection( A, B ) == true );
    }

};
}