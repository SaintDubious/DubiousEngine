#include "CppUnitTest.h"

#include <Utils.h>
#include <Ac3d_file_reader.h>
#include <Physics_model.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Dubious::Physics;
using namespace Dubious::Math;
using namespace Dubious::Utility;

namespace Physics_test
{		
TEST_CLASS(Physics_model_test) {
public:
		
	TEST_METHOD(radius_test)
	{
        std::unique_ptr<const Ac3d_file> model_file = Ac3d_file_reader::test_cube( 1.0f ); 

        std::shared_ptr<Physics_model> model = std::make_shared<Physics_model>( *model_file );
        Assert::IsTrue( equals( model->radius(), sqrt(3.0f) ) );

        model_file = Ac3d_file_reader::test_cube_group( 1.0f ); 
        model = std::make_shared<Physics_model>( *model_file );
        Assert::IsTrue( equals( model->radius(), 1.0f+sqrt(3.0f) ) );
    }
};
}