#include "CppUnitTest.h"

#include <Visible_model.h>
#include <Triple.h>
#include <Ac3d_file_reader.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Dubious::Renderer;
using namespace Dubious::Math;
using namespace Dubious::Utility;

namespace RendererTest
{		
TEST_CLASS(Visible_model_test)
{
public:
		
	TEST_METHOD(visible_model_construction)
	{
        std::unique_ptr<const Ac3d_file> model_file = Ac3d_file_reader::test_cube( 1.0f ); 
        Visible_model m1( *model_file, false );
        Assert::IsTrue( m1.m_points.size() == 8 );
        Assert::IsTrue( m1.m_surfaces.size() == 12 );
        Assert::IsTrue( m1.m_edges.size() == 0 );
        Assert::IsTrue( m1.m_surfaces[1].normal == Local_unit_vector( 1, 0, 0 ) );
        Visible_model m2( *model_file, true );
        Assert::IsTrue( m2.m_points.size() == 8 );
        Assert::IsTrue( m2.m_surfaces.size() == 12 );
        Assert::IsTrue( m2.m_edges.size() == 18 );
        Assert::IsTrue( m2.m_surfaces[1].normal == Local_unit_vector( 1, 0, 0 ) );
    }

};
}