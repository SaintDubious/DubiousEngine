#include "CppUnitTest.h"

#include <Physics_model.h>
#include <Physics_object.h>
#include <Collision_solver.h>
#include <Constraint_solver.h>
#include <Triple.h>
#include <Coordinate_space.h>
#include <Ac3d_file_reader.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Dubious::Physics;
using namespace Dubious::Math;
using namespace Dubious::Utility;

namespace Physics_test
{		
TEST_CLASS(Constraint_solver_test) {
public:
		
	TEST_METHOD(constraint_solver_solve)
	{
        Collision_solver collision_solver;
        Constraint_solver solver;
        std::vector<Collision_solver::Contact> contacts;
        std::unique_ptr<const Ac3d_file> model_file = Ac3d_file_reader::test_cube( 1.0f, 1.0f, 1.0f ); 

        std::shared_ptr<Physics_model> model = std::make_shared<Physics_model>( *model_file );
        Physics_object a( model, 1 );
        Physics_object b( model, 1 );

        b.coordinate_space().translate( Vector( 2, 0, 0 ) );
        b.velocity() = Vector( -10, 0, 0 );
        Assert::IsTrue( collision_solver.intersection( a, b, contacts ) == true );
        Constraint_solver::Velocity_matrix velocities = solver.solve( a, b, contacts );
        Assert::IsTrue( velocities.a_linear == Vector( -5, 0, 0 ) );
        Assert::IsTrue( velocities.b_linear == Vector( -5, 0, 0 ) );

        contacts.clear();
        b.coordinate_space().translate( Vector( 0, 1, 0 ) );
        Assert::IsTrue( collision_solver.intersection( a, b, contacts ) == true );
        velocities = solver.solve( a, b, contacts );
    }
};
}
