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
        std::unique_ptr<const Ac3d_file> model_file = Ac3d_file_reader::test_cube( 1.0f );
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
};
}
