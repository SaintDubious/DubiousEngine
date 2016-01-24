#include "Physics_model.h"

#include <Ac3d_file_reader.h>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

//////////////////////////////////////////////////////////////
Physics_model::Physics_model( const Utility::Ac3d_file& file )
{
    construct( Math::Local_vector(), *file.model() );
}

//////////////////////////////////////////////////////////////
Physics_model::Physics_model( const Math::Triple& Dimensions )
{
    m_vectors.push_back( Math::Local_vector( -Dimensions.m_x, -Dimensions.m_y, -Dimensions.m_z ) );
    m_vectors.push_back( Math::Local_vector(  Dimensions.m_x, -Dimensions.m_y, -Dimensions.m_z ) );
    m_vectors.push_back( Math::Local_vector(  Dimensions.m_x, -Dimensions.m_y,  Dimensions.m_z ) );
    m_vectors.push_back( Math::Local_vector( -Dimensions.m_x, -Dimensions.m_y,  Dimensions.m_z ) );
    m_vectors.push_back( Math::Local_vector( -Dimensions.m_x,  Dimensions.m_y,  Dimensions.m_z ) );
    m_vectors.push_back( Math::Local_vector(  Dimensions.m_x,  Dimensions.m_y,  Dimensions.m_z ) );
    m_vectors.push_back( Math::Local_vector(  Dimensions.m_x,  Dimensions.m_y, -Dimensions.m_z ) );
    m_vectors.push_back( Math::Local_vector( -Dimensions.m_x,  Dimensions.m_y, -Dimensions.m_z ) );
}

//////////////////////////////////////////////////////////////
void Physics_model::construct( const Math::Local_vector& offset, const Utility::Ac3d_model& model )
{
    Math::Local_vector new_offset = offset + (model.offset()-Math::Local_point());
    for (const auto& p : model.points()) {
        m_vectors.push_back( new_offset + (p-Math::Local_point()) );
    }

    for (const auto& kid : model.kids()) {
        m_kids.push_back( std::unique_ptr<Physics_model>(new Physics_model) );
        m_kids.back()->construct( new_offset, *kid );
    }
}

}}
