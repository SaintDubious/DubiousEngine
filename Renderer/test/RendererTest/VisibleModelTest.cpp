#include "CppUnitTest.h"

#include <VisibleModel.h>
#include <Triple.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Dubious::Renderer;
using namespace Dubious::Math;

namespace RendererTest
{		
TEST_CLASS(VisibleModelTest)
{
public:
		
	TEST_METHOD(VisibleModelConstruction)
	{
        VisibleModel M1( Triple( 1, 1, 1 ), false );
        Assert::IsTrue( M1.m_Points.size() == 8 );
        Assert::IsTrue( M1.m_Surfaces.size() == 12 );
        Assert::IsTrue( M1.m_Edges.size() == 0 );
        Assert::IsTrue( M1.m_Surfaces[1].Normal == LocalUnitVector( 1, 0, 0 ) );
        VisibleModel M2( Triple( 1, 1, 1 ), true );
        Assert::IsTrue( M2.m_Points.size() == 8 );
        Assert::IsTrue( M2.m_Surfaces.size() == 12 );
        Assert::IsTrue( M2.m_Edges.size() == 18 );
        Assert::IsTrue( M2.m_Surfaces[1].Normal == LocalUnitVector( 1, 0, 0 ) );
    }

};
}