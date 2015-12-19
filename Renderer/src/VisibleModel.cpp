#include "VisibleModel.h"
#include <AC3DFileReader.h>
#include <VectorMath.h>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

//////////////////////////////////////////////////////////////
VisibleModel::VisibleModel()
    : m_Color( Color::WHITE )
{
}

//////////////////////////////////////////////////////////////
VisibleModel::VisibleModel( const Utility::AC3DFile& File, bool IncludeEdges )
    : m_Color( Color::WHITE )
{
    Construct( *File.Model(), File.Materials(), IncludeEdges );
}

namespace 
{
VisibleModel::Surface BuildSurface( const Math::LocalPointVector& Points, int p0, int p1, int p2 )
{
    const Math::LocalPoint& A = Points[p0];
    const Math::LocalPoint& B = Points[p1];
    const Math::LocalPoint& C = Points[p2];
    return VisibleModel::Surface( p0, p1, p2, Math::CrossProduct( (B-A), (C-A) ) );
}

}

//////////////////////////////////////////////////////////////
void VisibleModel::Construct( const Utility::AC3DModel& AC3DModel, const Utility::AC3DMaterialVector& Materials, bool IncludeEdges )
{
    m_Offset = AC3DModel.Offset();
    m_Points = AC3DModel.Points();
    for (const Utility::AC3DModel::Surface& S : AC3DModel.Surfaces()) {
        m_Surfaces.push_back( BuildSurface( m_Points, S.p0, S.p1, S.p2 ) );
    }
    for (Utility::AC3DModelPtr p : AC3DModel.Kids()) {
        m_Kids.push_back( VisibleModelPtr( new VisibleModel() ) );
        m_Kids.back()->Construct( *p, Materials, IncludeEdges );
    }
    if (!AC3DModel.Surfaces().empty()) {
        const Utility::AC3DMaterial& Material = Materials[AC3DModel.Surfaces()[0].MaterialIndex];
        m_Color = Color( Material.MaterialColor().Red, Material.MaterialColor().Green, Material.MaterialColor().Blue, 1.0f );
    }
//	ComputePointNormals();
    if (IncludeEdges) {
        BuildEdges();
    }
}

//////////////////////////////////////////////////////////////
VisibleModel::VisibleModel( const Math::Triple& Dimensions, bool IncludeEdges )
{
    // The code for this comes from looking at an AC3D file
    // for a cube.
    m_Points.push_back( Math::LocalPoint( -Dimensions.m_X, -Dimensions.m_Y, -Dimensions.m_Z ) );
    m_Points.push_back( Math::LocalPoint(  Dimensions.m_X, -Dimensions.m_Y, -Dimensions.m_Z ) );
    m_Points.push_back( Math::LocalPoint(  Dimensions.m_X, -Dimensions.m_Y,  Dimensions.m_Z ) );
    m_Points.push_back( Math::LocalPoint( -Dimensions.m_X, -Dimensions.m_Y,  Dimensions.m_Z ) );
    m_Points.push_back( Math::LocalPoint( -Dimensions.m_X,  Dimensions.m_Y,  Dimensions.m_Z ) );
    m_Points.push_back( Math::LocalPoint(  Dimensions.m_X,  Dimensions.m_Y,  Dimensions.m_Z ) );
    m_Points.push_back( Math::LocalPoint(  Dimensions.m_X,  Dimensions.m_Y, -Dimensions.m_Z ) );
    m_Points.push_back( Math::LocalPoint( -Dimensions.m_X,  Dimensions.m_Y, -Dimensions.m_Z ) );

    m_Surfaces.push_back( BuildSurface( m_Points, 6, 2, 1 ) );
    m_Surfaces.push_back( BuildSurface( m_Points, 2, 6, 5 ) );
    m_Surfaces.push_back( BuildSurface( m_Points, 4, 0, 3 ) );
    m_Surfaces.push_back( BuildSurface( m_Points, 0, 4, 7 ) );
    m_Surfaces.push_back( BuildSurface( m_Points, 6, 0, 7 ) );
    m_Surfaces.push_back( BuildSurface( m_Points, 0, 6, 1 ) );
    m_Surfaces.push_back( BuildSurface( m_Points, 2, 4, 3 ) );
    m_Surfaces.push_back( BuildSurface( m_Points, 4, 2, 5 ) );
    m_Surfaces.push_back( BuildSurface( m_Points, 4, 6, 7 ) );
    m_Surfaces.push_back( BuildSurface( m_Points, 6, 4, 5 ) );
    m_Surfaces.push_back( BuildSurface( m_Points, 2, 0, 1 ) );
    m_Surfaces.push_back( BuildSurface( m_Points, 0, 2, 3 ) );

    m_Color = Color( 1.0, 1.0, 1.0, 1.0 );
//	ComputePointNormals();
    if (IncludeEdges) {
        BuildEdges();
    }
}

//////////////////////////////////////////////////////////////
void VisibleModel::ComputePointNormals()
{
    for( size_t i=0; i<m_Points.size(); ++i ){
        Math::LocalVector AvgNormal;
        for (const Surface& S : m_Surfaces) {
            if ((S.p0 == i) || (S.p1 == i) || (S.p2 == i) ){
                // this surface includes the vertex in question.
                // find its normal
                AvgNormal = AvgNormal + Math::LocalVector( S.Normal );
            }
        }
        m_PointNormals.push_back( Math::LocalUnitVector( AvgNormal ) );
    }
}

//////////////////////////////////////////////////////////////
void VisibleModel::BuildEdges()
{
    size_t j = 0;
    for( size_t i=0; i<m_Surfaces.size(); ++i ){
        unsigned short i0 = m_Surfaces[i].p0;
        unsigned short i1 = m_Surfaces[i].p1;
        for( j=i+1; j<m_Surfaces.size(); ++j ){
            if( ((m_Surfaces[j].p0==i0) && ((m_Surfaces[j].p1==i1) || (m_Surfaces[j].p2==i1))) ||
                ((m_Surfaces[j].p1==i0) && ((m_Surfaces[j].p0==i1) || (m_Surfaces[j].p2==i1))) ||
                ((m_Surfaces[j].p2==i0) && ((m_Surfaces[j].p0==i1) || (m_Surfaces[j].p1==i1))) ){
                AddEdge( i0, i1, static_cast<unsigned short>(i), static_cast<unsigned short>(j) );
                break;
            }
        }
        i0 = m_Surfaces[i].p1;
        i1 = m_Surfaces[i].p2;
        for( j=i+1; j<m_Surfaces.size(); ++j ){
            if( ((m_Surfaces[j].p0==i0) && ((m_Surfaces[j].p1==i1) || (m_Surfaces[j].p2==i1))) ||
                ((m_Surfaces[j].p1==i0) && ((m_Surfaces[j].p0==i1) || (m_Surfaces[j].p2==i1))) ||
                ((m_Surfaces[j].p2==i0) && ((m_Surfaces[j].p0==i1) || (m_Surfaces[j].p1==i1))) ){
                AddEdge( i0, i1, static_cast<unsigned short>(i), static_cast<unsigned short>(j) );
                break;
            }
        }
        i0 = m_Surfaces[i].p2;
        i1 = m_Surfaces[i].p0;
        for( j=i+1; j<m_Surfaces.size(); ++j ){
            if( ((m_Surfaces[j].p0==i0) && ((m_Surfaces[j].p1==i1) || (m_Surfaces[j].p2==i1))) ||
                ((m_Surfaces[j].p1==i0) && ((m_Surfaces[j].p0==i1) || (m_Surfaces[j].p2==i1))) ||
                ((m_Surfaces[j].p2==i0) && ((m_Surfaces[j].p0==i1) || (m_Surfaces[j].p1==i1))) ){
                AddEdge( i0, i1, static_cast<unsigned short>(i), static_cast<unsigned short>(j) );
                break;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////
void VisibleModel::AddEdge( unsigned short p0, unsigned short p1, unsigned short s0, unsigned short s1 )
{
    Edge	NewEdge;
    NewEdge.p0 = p0;
    NewEdge.p1 = p1;
    NewEdge.s0 = s0;
    NewEdge.s1 = s1;
    m_Edges.push_back( NewEdge );
}

}}