#include "PhysicsModel.h"

#include <AC3DFileReader.h>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

//////////////////////////////////////////////////////////////
PhysicsModel::PhysicsModel( const Utility::AC3DFile& File )
{
    Construct( *File.Model() );
}

//////////////////////////////////////////////////////////////
PhysicsModel::PhysicsModel( const Math::Triple& Dimensions )
{
    m_Points.push_back( Math::LocalPoint( -Dimensions.m_X, -Dimensions.m_Y, -Dimensions.m_Z ) );
    m_Points.push_back( Math::LocalPoint(  Dimensions.m_X, -Dimensions.m_Y, -Dimensions.m_Z ) );
    m_Points.push_back( Math::LocalPoint(  Dimensions.m_X, -Dimensions.m_Y,  Dimensions.m_Z ) );
    m_Points.push_back( Math::LocalPoint( -Dimensions.m_X, -Dimensions.m_Y,  Dimensions.m_Z ) );
    m_Points.push_back( Math::LocalPoint( -Dimensions.m_X,  Dimensions.m_Y,  Dimensions.m_Z ) );
    m_Points.push_back( Math::LocalPoint(  Dimensions.m_X,  Dimensions.m_Y,  Dimensions.m_Z ) );
    m_Points.push_back( Math::LocalPoint(  Dimensions.m_X,  Dimensions.m_Y, -Dimensions.m_Z ) );
    m_Points.push_back( Math::LocalPoint( -Dimensions.m_X,  Dimensions.m_Y, -Dimensions.m_Z ) );
}

//////////////////////////////////////////////////////////////
void PhysicsModel::Construct( const Utility::AC3DModel& AC3DModel )
{
    m_Offset = AC3DModel.Offset();
    m_Points = AC3DModel.Points();

    for (Utility::AC3DModelPtr p : AC3DModel.Kids()) {
        m_Kids.push_back( PhysicsModelPtr( new PhysicsModel() ) );
        m_Kids.back()->Construct( *p );
    }
}

}}
