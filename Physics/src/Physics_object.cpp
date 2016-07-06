#include "Physics_object.h"
#include "Physics_model.h"

#include <Vector.h>
#include <Vector_math.h>
//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

const float Physics_object::STATIONARY = -1;

//////////////////////////////////////////////////////////////
Physics_object::Physics_object( const std::shared_ptr<Physics_model>& model, float mass )
    : m_model( model )
    , m_mass( mass )
{
    // This is a cheat. We just use moment of inertia of a sphere
    // 2/5 * m * r * r
    // https://en.wikipedia.org/wiki/List_of_moments_of_inertia
    m_moment_of_inertia = (2.0f/5.0f) * m_mass * m_model->radius() * m_model->radius();
}

//////////////////////////////////////////////////////////////
namespace {
void copy_model_to_vectors( const Physics_model& model, Math::Coordinate_space coords, std::vector<Math::Vector>& vectors )
{
    for (const auto& v : model.vectors()) {
        Math::Vector new_vect = coords.transform( v );
        new_vect = new_vect + Math::to_vector(coords.position());
        vectors.push_back( new_vect );
    }
}
}

}}
