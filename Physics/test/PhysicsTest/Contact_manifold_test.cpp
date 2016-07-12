#include "CppUnitTest.h"

#include <Contact_manifold.h>
#include <Ac3d_file_reader.h>
#include <Physics_model.h>
#include <Physics_object.h>
#include <Collision_solver.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Dubious::Physics;
using namespace Dubious::Math;
using namespace Dubious::Utility;

namespace Physics_test
{		
TEST_CLASS(Contact_manifold_test) {
public:

	TEST_METHOD(contact_manifold_prune_test)
	{
        Collision_solver solver;

        std::unique_ptr<const Ac3d_file> model_file = Ac3d_file_reader::test_cube( 1.0f, 1.0f, 1.0f ); 

        std::shared_ptr<Physics_model> model = std::make_shared<Physics_model>( *model_file );
        std::shared_ptr<Physics_object> a( new Physics_object( model, 1 ) );
        std::shared_ptr<Physics_object> b( new Physics_object( model, 1 ) );
        b->coordinate_space().position() = Point( 0, 2, 0 );

        Contact_manifold contact_manifold( a, b );
        std::vector<Contact_manifold::Contact> contacts;
        Assert::IsTrue( solver.intersection( *a, *b, contacts ) == true );
        contact_manifold.insert( contacts );
        contact_manifold.prune_old_contacts();
        Assert::IsTrue( contact_manifold.contacts().size() == 1 );

        b->coordinate_space().position() = Point( 0, 2.1f, 0 );
        contact_manifold.prune_old_contacts();
        Assert::IsTrue( contact_manifold.contacts().size() == 0 );

        contacts.clear();
        b->coordinate_space().position() = Point( 0, 1.8f, 0 );
        Assert::IsTrue( solver.intersection( *a, *b, contacts ) == true );
        contact_manifold.insert( contacts );
        contact_manifold.prune_old_contacts();
        Assert::IsTrue( contact_manifold.contacts().size() == 1 );

        b->coordinate_space().rotate( Quaternion( Unit_vector( 0, 0, 1 ), to_radians( 10 ) ) );
        contact_manifold.prune_old_contacts();
        Assert::IsTrue( contact_manifold.contacts().size() == 0 );
    }

};
}