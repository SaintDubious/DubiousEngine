#include "Physics_object.h"
#include "Physics_model.h"
#include "Vector.h"

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

//////////////////////////////////////////////////////////////
Physics_object::Physics_object( const std::shared_ptr<Physics_model>& model )
    : m_model( model )
{
}

//////////////////////////////////////////////////////////////
namespace {
void copy_model_to_vectors( const Physics_model& model, Math::Coordinate_space coords, std::vector<Math::Vector>& vectors )
{
    Math::Local_vector offset = model.offset() - Math::Local_point();
    for (const auto& point : model.points()) {
        Math::Vector new_vect = coords.transform( offset + (point-Math::Local_point()) );
        new_vect = new_vect + (coords.position() - Math::Point());
        vectors.push_back( new_vect );
    }
    coords.translate( offset );
    for (const auto& kid : model.kids()) {
        copy_model_to_vectors( *kid, coords, vectors );
    }
}
}

//////////////////////////////////////////////////////////////
void Physics_object::cache_collision_vectors() const
{
    if (m_coordinate_space == m_cached_coordinate_space && !m_cached_collision_vectors.empty()) {
        return;
    }
    m_cached_collision_vectors.clear();
    copy_model_to_vectors( *m_model, m_coordinate_space, m_cached_collision_vectors );
    m_cached_coordinate_space = m_coordinate_space;
}

}}
