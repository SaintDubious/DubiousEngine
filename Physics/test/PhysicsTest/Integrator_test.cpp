#include "CppUnitTest.h"

#include <Ac3d_file_reader.h>
#include <Physics_model.h>
#include <Physics_object.h>
#include <Integrator.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Dubious::Physics;
using namespace Dubious::Math;
using namespace Dubious::Utility;

namespace Physics_test
{		
TEST_CLASS(Integrator_test) {
public:
		
	TEST_METHOD(integrator_linear)
	{
        std::unique_ptr<const Ac3d_file> model_file = Ac3d_file_reader::test_cube( 1.0f, 1.0f, 1.0f );
        std::shared_ptr<Physics_model> model = std::make_shared<Physics_model>( *model_file );
        Physics_object object( model, 1.0f );

        Integrator integrator( 0.5f );
        integrator.integrate_linear( object, 0.5 );
        Assert::IsTrue( object.velocity() == Vector() );

        object.force() = Vector( 1, 0, 0 );
        integrator.integrate_linear( object, 0.5 );
        Assert::IsTrue( object.velocity() == Vector( 0.5f, 0, 0 ) );
        Assert::IsTrue( object.coordinate_space().position() == Point( 0.125f, 0, 0 ) );
        integrator.integrate_linear( object, 0.5 );
        Assert::IsTrue( object.velocity() == Vector( 1.0f, 0, 0 ) );
        Assert::IsTrue( object.coordinate_space().position() == Point( 0.5f, 0, 0 ) );
    }

	TEST_METHOD(integrator_angular)
	{
        std::unique_ptr<const Ac3d_file> model_file = Ac3d_file_reader::test_cube( 1.0f, 1.0f, 1.0f );
        std::shared_ptr<Physics_model> model = std::make_shared<Physics_model>( *model_file );
        Physics_object object( model, 1.0f );

        Integrator integrator( 0.5f );
        integrator.integrate_angular( object, 0.5 );
        Assert::IsTrue( object.angular_velocity() == Vector() );

        object.torque() = Vector( 1, 0, 0 );
        integrator.integrate_angular( object, 0.5 );
        Assert::IsTrue( object.angular_velocity() == Vector( 0.416666687f, 0, 0 ) );

        object.angular_velocity() = Vector();
        object.coordinate_space().rotation() = Quaternion( Unit_vector( 0, 1, 0 ), to_radians( 90 ) );
        integrator.integrate_angular( object, 0.5 );
        Assert::IsTrue( object.angular_velocity() == Vector( 0.416666687f, 0, 0 ) );
    }

	TEST_METHOD(integrator_stationary)
	{
        std::unique_ptr<const Ac3d_file> model_file = Ac3d_file_reader::test_cube( 1.0f, 1.0f, 1.0f );
        std::shared_ptr<Physics_model> model = std::make_shared<Physics_model>( *model_file );
        std::vector<std::shared_ptr<Physics_object>> objects = {std::make_shared<Physics_object>( model, Physics_object::STATIONARY )};

        Integrator integrator( 0.5f );
        integrator.update( objects, 0.5 );
        Assert::IsTrue( objects[0]->velocity() == Vector() );
        objects[0]->force() = Vector( 1, 0, 0 );
        integrator.update( objects, 0.5 );
        Assert::IsTrue( objects[0]->velocity() == Vector() );
        Assert::IsTrue( objects[0]->coordinate_space().position() == Point() );
    }

};
}
