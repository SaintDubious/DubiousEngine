#include "Physics_model.h"

#include <Ac3d_file_reader.h>
#include <Vector_math.h>

#include <algorithm>

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
        Math::Local_vector v = Math::to_vector(p);
        m_radius = std::max(m_radius,v.length_squared());
        m_vectors.push_back( new_offset + v );
    }
    m_radius = std::sqrt(m_radius);

    for (const auto& kid : model.kids()) {
        m_kids.push_back( std::unique_ptr<Physics_model>(new Physics_model) );
        m_kids.back()->construct( new_offset, *kid );
        m_radius = std::max(m_radius,Math::to_vector(kid->offset()).length()+m_kids.back()->radius());
    }
}

}}
