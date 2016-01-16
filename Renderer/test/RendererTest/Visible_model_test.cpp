#include "CppUnitTest.h"

#include <Visible_model.h>
#include <Triple.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Dubious::Renderer;
using namespace Dubious::Math;

namespace RendererTest
{		
TEST_CLASS(Visible_model_test)
{
public:
		
	TEST_METHOD(visible_model_construction)
	{
        Visible_model m1( Triple( 1, 1, 1 ), false );
        Assert::IsTrue( m1.m_points.size() == 8 );
        Assert::IsTrue( m1.m_surfaces.size() == 12 );
        Assert::IsTrue( m1.m_edges.size() == 0 );
        Assert::IsTrue( m1.m_surfaces[1].normal == Local_unit_vector( 1, 0, 0 ) );
        Visible_model m2( Triple( 1, 1, 1 ), true );
        Assert::IsTrue( m2.m_points.size() == 8 );
        Assert::IsTrue( m2.m_surfaces.size() == 12 );
        Assert::IsTrue( m2.m_edges.size() == 18 );
        Assert::IsTrue( m2.m_surfaces[1].normal == Local_unit_vector( 1, 0, 0 ) );
    }

};
}