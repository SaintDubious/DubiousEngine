#include "Physics_model.h"

#include <Ac3d_file_reader.h>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

//////////////////////////////////////////////////////////////
Physics_model::Physics_model( const Utility::Ac3d_file& file )
{
    construct( *file.model() );
}

//////////////////////////////////////////////////////////////
Physics_model::Physics_model( const Math::Triple& Dimensions )
{
    m_points.push_back( Math::Local_point( -Dimensions.m_x, -Dimensions.m_y, -Dimensions.m_z ) );
    m_points.push_back( Math::Local_point(  Dimensions.m_x, -Dimensions.m_y, -Dimensions.m_z ) );
    m_points.push_back( Math::Local_point(  Dimensions.m_x, -Dimensions.m_y,  Dimensions.m_z ) );
    m_points.push_back( Math::Local_point( -Dimensions.m_x, -Dimensions.m_y,  Dimensions.m_z ) );
    m_points.push_back( Math::Local_point( -Dimensions.m_x,  Dimensions.m_y,  Dimensions.m_z ) );
    m_points.push_back( Math::Local_point(  Dimensions.m_x,  Dimensions.m_y,  Dimensions.m_z ) );
    m_points.push_back( Math::Local_point(  Dimensions.m_x,  Dimensions.m_y, -Dimensions.m_z ) );
    m_points.push_back( Math::Local_point( -Dimensions.m_x,  Dimensions.m_y, -Dimensions.m_z ) );
}

//////////////////////////////////////////////////////////////
void Physics_model::construct( const Utility::Ac3d_model& model )
{
    m_offset = model.offset();
    m_points = model.points();

    for (const auto& kid : model.kids()) {
        m_kids.push_back( std::unique_ptr<Physics_model>(new Physics_model) );
        m_kids.back()->construct( *kid );
    }
}

}}
