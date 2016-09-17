#include "CppUnitTest.h"

#include <Contact_manifold.h>
#include <Ac3d_file_reader.h>
#include <Physics_model.h>
#include <Physics_object.h>
#include <Collision_solver.h>
#include <Point.h>
#include <Vector_math.h>

#include <tuple>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Dubious::Physics;
using namespace Dubious::Math;
using namespace Dubious::Utility;

namespace Physics_test
{		
TEST_CLASS(Contact_manifold_test) {
public:

	TEST_METHOD(contact_manifold_insert_test)
	{
        Collision_solver solver;

        std::unique_ptr<const Ac3d_file> model_file = Ac3d_file_reader::test_cube( 1.0f, 1.0f, 1.0f ); 

        std::shared_ptr<Physics_model> model = std::make_shared<Physics_model>( *model_file );
        std::shared_ptr<Physics_object> a( new Physics_object( model, 1 ) );
        std::shared_ptr<Physics_object> b( new Physics_object( model, 1 ) );
        b->coordinate_space().position() = Point( 0, 2, 0 );

        Contact_manifold contact_manifold( a, b, 0.05f );
        std::vector<Contact_manifold::Contact> contacts;
        Assert::IsTrue( solver.intersection( *a, *b, contacts ) == true );
        contact_manifold.insert( contacts );
        Assert::IsTrue( contact_manifold.contacts().size() == 1 );
        contact_manifold.insert( contacts );
        Assert::IsTrue( contact_manifold.contacts().size() == 1 );
        contacts[0].contact_point_a = Point( 100, 100, 100 );
        contact_manifold.insert( contacts );
        Assert::IsTrue( contact_manifold.contacts().size() == 2 );
    }

	TEST_METHOD(contact_manifold_prune_test)
	{
        Collision_solver solver;

        std::unique_ptr<const Ac3d_file> model_file = Ac3d_file_reader::test_cube( 1.0f, 1.0f, 1.0f ); 

        std::shared_ptr<Physics_model> model = std::make_shared<Physics_model>( *model_file );
        std::shared_ptr<Physics_object> a( new Physics_object( model, 1 ) );
        std::shared_ptr<Physics_object> b( new Physics_object( model, 1 ) );
        b->coordinate_space().position() = Point( 0, 2, 0 );

        Contact_manifold contact_manifold( a, b, 0.05f );
        std::vector<Contact_manifold::Contact> contacts;
        Assert::IsTrue( solver.intersection( *a, *b, contacts ) == true );
        contact_manifold.insert( contacts );
        contact_manifold.prune_old_contacts();
        Assert::IsTrue( contact_manifold.contacts().size() == 1 );

        b->coordinate_space().position() = Point( 0, 2.2f, 0 );
        contact_manifold.prune_old_contacts();
        Assert::IsTrue( contact_manifold.contacts().size() == 0 );

        contacts.clear();
        b->coordinate_space().position() = Point( 0, 1.8f, 0 );
        Assert::IsTrue( solver.intersection( *a, *b, contacts ) == true );
        contact_manifold.insert( contacts );
        contact_manifold.prune_old_contacts();
        Assert::IsTrue( contact_manifold.contacts().size() == 1 );

        b->coordinate_space().rotate( Quaternion( Unit_vector( 0, 0, 1 ), to_radians( 20 ) ) );
        contact_manifold.prune_old_contacts();
        Assert::IsTrue( contact_manifold.contacts().size() == 0 );
    }

	TEST_METHOD(contact_manifold_distance_squared_to_line_segment_test)
	{
        // Methodology:
        // I created a blender scene that contains a line segment with a handful
        // of points at various locations around the segment. Then I rotated 
        // the model and plugged in the points
        std::unique_ptr<const Ac3d_file> model_file = Ac3d_file_reader::test_cube( 1.0f, 1.0f, 1.0f ); 

        std::shared_ptr<Physics_model> model = std::make_shared<Physics_model>( *model_file );
        std::shared_ptr<Physics_object> a( new Physics_object( model, 1 ) );
        std::shared_ptr<Physics_object> b( new Physics_object( model, 1 ) );

        Contact_manifold contact_manifold( a, b, 0.05f );
        
        float d = 0;
        // Off of one end of the line
        d = contact_manifold.distance_squared_to_line_segment( Point( 0.90881f, -0.93522f, 2.15244f ), 
                                                               Point( 1.80702f, 0.71604f, 1.46937f ), 
                                                               Point( 2.71719f, 1.65494f, 2.00794f ) );
        Assert::IsTrue( equals( d, 2 ) );
        d = contact_manifold.distance_squared_to_line_segment( Point( 0.90881f, -0.93522f, 2.15244f ), 
                                                               Point( 1.80702f, 0.71604f, 1.46937f ), 
                                                               Point( 1.95186f, 2.20766f, 2.33774f ) );
        Assert::IsTrue( equals( d, 2.99997520f ) );
        // In the middle of the line
        d = contact_manifold.distance_squared_to_line_segment( Point( 0.90881f, -0.93522f, 2.15244f ), 
                                                               Point( 1.80702f, 0.71604f, 1.46937f ), 
                                                               Point( 1.05366f, 0.55641f, 3.02081f ) );
        Assert::IsTrue( equals( d, 1.99999690f ) );
        // Off the other end of the line
        d = contact_manifold.distance_squared_to_line_segment( Point( 0.90881f, -0.93522f, 2.15244f ), 
                                                               Point( 1.80702f, 0.71604f, 1.46937f ), 
                                                               Point( -0.76668f, -1.32139f, 1.94366f ) );
        Assert::IsTrue( equals( d, 2.99998307f ) );
    }

	TEST_METHOD(contact_manifold_triangle_to_zy_plane_test)
	{
        // Methodology:
        // I created some triangles in Blender and rotated them around for test points
        // then create the coordinate space to rotate them back, then test it out
        std::unique_ptr<const Ac3d_file> model_file = Ac3d_file_reader::test_cube( 1.0f, 1.0f, 1.0f ); 

        std::shared_ptr<Physics_model> model = std::make_shared<Physics_model>( *model_file );
        std::shared_ptr<Physics_object> a( new Physics_object( model, 1 ) );
        std::shared_ptr<Physics_object> b( new Physics_object( model, 1 ) );

        Contact_manifold contact_manifold( a, b, 0.05f );
        Coordinate_space cs;
        {
            // simple test, no translation or rotation
            cs = contact_manifold.triangle_to_zy_plane( Point( 0, 0, 0 ),
                                                        Point( 0, 0, 1 ),
                                                        Point( 0, 1, 0 ) );
            Assert::IsTrue( cs.position() == Point() );
            Assert::IsTrue( cs.rotation() == Quaternion() );
        }
        {
            // This is a right triangle with:
            // a = (0,0,0)
            // b = (0,0,2)
            // c = (0,1,0)
            cs = contact_manifold.triangle_to_zy_plane( Point( 0.59903f, 1.01561f, 0.65871f ),
                                                        Point( 0.14958f, -0.00496f, 2.31896f ),
                                                        Point( -0.19019f, 0.61125f, 0.1965f ) );

            Assert::IsTrue( cs.position() == to_point(Vector( 0.59903f, 1.01561f, 0.65871f ) ));
            Local_point lp = cs.transform( Point( 0.14958f, -0.00496f, 2.31896f ) );
            Assert::IsTrue( lp == Local_point( 0, 0, 2 ) );
            Point p = cs.transform( Local_point( 0, 0, 2 ) );
            Assert::IsTrue( p == Point( 0.14958f, -0.00496f, 2.31896f ) );
            lp = cs.transform( Point( -0.19019f, 0.61125f, 0.1965f ) );
            Assert::IsTrue(lp == Local_point( -5.62053174e-07f, 1.00000668f, 4.19560820e-06f ) );
        }
        {
            // This is an obtuse triangle with:
            // a = (0,0,0)
            // b = (0,0,2)
            // c = (0,1,-1)
            cs = contact_manifold.triangle_to_zy_plane( Point( 1.92513f, -0.67394f, 1.49395f ),
                                                        Point( 2.68395f, -0.11228f, -0.26921f ),
                                                        Point( 0.98195f, -1.64011f, 1.91458f ) );
            Assert::IsTrue( cs.position() == to_point(Vector( 1.92513f, -0.67394f, 1.49395f ) ));
            Local_point lp = cs.transform( Point( 2.68395f, -0.11228f, -0.26921f ) );
            Assert::IsTrue( lp == Local_point( 0, 0, 2 ) );
            lp = cs.transform( Point( 0.98195f, -1.64011f, 1.91458f ) );
            Assert::IsTrue(lp == Local_point( -5.96046448e-08f, 1.00000143f, -0.999999881f ) );
        } 
        {
            // This is an accute triangle with:
            // a = (0,0,0)
            // b = (0,-1,2)
            // c = (0,1,2)
            // NOTE: This is the first one where the transformed output does not equal the 
            // original input. This is becaue point b is made to lie on the z axis, so 
            // clearly it couldn't come out to (0,-1,2)
            cs = contact_manifold.triangle_to_zy_plane( Point( 0.00375f, 1.20145f, 0.848f ),
                                                        Point( -0.25845f, -0.6658f, -0.35392f ),
                                                        Point( 1.62083f, -0.14227f, 0.08677f ) );
            Assert::IsTrue( cs.position() == to_point(Vector( 0.00375f, 1.20145f, 0.848f ) ));
            Local_point lp = cs.transform( Point( -0.25845f, -0.6658f, -0.35392f ) );
            Assert::IsTrue( lp == Local_point( -1.49011612e-07f, -2.38418579e-07f, 2.23606420f ) );
            lp = cs.transform( Point( 1.62083f, -0.14227f, 0.08677f ) );
            Assert::IsTrue(lp == Local_point( -2.98023224e-07f, 1.78885317f, 1.34164333f ) );
        }
        
    }

	TEST_METHOD(contact_manifold_closest_segment)
	{
        std::unique_ptr<const Ac3d_file> model_file = Ac3d_file_reader::test_cube( 1.0f, 1.0f, 1.0f ); 

        std::shared_ptr<Physics_model> model = std::make_shared<Physics_model>( *model_file );
        std::shared_ptr<Physics_object> model_a( new Physics_object( model, 1 ) );
        std::shared_ptr<Physics_object> model_b( new Physics_object( model, 1 ) );

        Contact_manifold contact_manifold( model_a, model_b, 0.05f );
        
        Point a( -1, -1, 0 );
        Point b( 1, -1, 0 );
        Point c( 1, 1, 0 );
        
        bool intersect;
        Point r1, r2;
        // Simple test
        std::tie(intersect, r1, r2) = contact_manifold.closest_segment( a, b, c, Point( 0, -2, 0 ) );
        Assert::IsTrue( intersect && (r1==a) && (r2==b) );
        std::tie(intersect, r1, r2) = contact_manifold.closest_segment( a, b, c, Point( 3, 0, 0 ) );
        Assert::IsTrue( intersect && (r1==b) && (r2==c) );
        std::tie(intersect, r1, r2) = contact_manifold.closest_segment( a, b, c, Point( -1, 1, 0 ) );
        Assert::IsTrue( intersect && (r1==a) && (r2==c) );
        std::tie(intersect, r1, r2) = contact_manifold.closest_segment( a, b, c, Point( 0, 0, 0 ) );
        Assert::IsTrue( !intersect );

        // same thing, but triangle wound in the other direction 
        std::tie(intersect, r1, r2) = contact_manifold.closest_segment( a, c, b, Point( 0, -2, 0 ) );
        Assert::IsTrue( intersect && (r1==a) && (r2==b) );
        std::tie(intersect, r1, r2) = contact_manifold.closest_segment( a, c, b, Point( 3, 0, 0 ) );
        Assert::IsTrue( intersect && (r1==c) && (r2==b) );
        std::tie(intersect, r1, r2) = contact_manifold.closest_segment( a, c, b, Point( -1, 1, 0 ) );
        Assert::IsTrue( intersect && (r1==a) && (r2==c) );
        std::tie(intersect, r1, r2) = contact_manifold.closest_segment( a, c, b, Point( 0, 0, 0 ) );
        Assert::IsTrue( !intersect );

        // Now with more Z
        a = Point( -1, -1, -5 );
        b = Point( 1, -1, 5 );
        c = Point( 1, 1, 2 );
        std::tie(intersect, r1, r2) = contact_manifold.closest_segment( a, b, c, Point( 0, -2, 0 ) );
        Assert::IsTrue( intersect && (r1==a) && (r2==b) );
        std::tie(intersect, r1, r2) = contact_manifold.closest_segment( a, b, c, Point( 3, 0, 0 ) );
        Assert::IsTrue( intersect && (r1==a) && (r2==c) );
        std::tie(intersect, r1, r2) = contact_manifold.closest_segment( a, b, c, Point( -1, 1, 0 ) );
        Assert::IsTrue( intersect && (r1==a) && (r2==c) );
        std::tie(intersect, r1, r2) = contact_manifold.closest_segment( a, b, c, Point( 0, 0, 0 ) );
        Assert::IsTrue( !intersect );
    }

};
}