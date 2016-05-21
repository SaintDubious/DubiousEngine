#include "Physics_model.h"

#include <Ac3d_file_reader.h>
#include <Vector_math.h>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

//////////////////////////////////////////////////////////////
Physics_model::Physics_model( const Utility::Ac3d_file& file )
{
    construct( Math::Local_vector(), *file.model() );
}

//////////////////////////////////////////////////////////////
void Physics_model::construct( const Math::Local_vector& offset, const Utility::Ac3d_model& model )
{
    Math::Local_vector new_offset = offset + (Math::to_vector(model.offset()));
    for (const auto& p : model.points()) {
        m_vectors.push_back( new_offset + (Math::to_vector(p)) );
    }

    for (const auto& kid : model.kids()) {
        m_kids.push_back( std::unique_ptr<Physics_model>(new Physics_model) );
        m_kids.back()->construct( new_offset, *kid );
    }
}

}}
