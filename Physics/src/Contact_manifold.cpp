#include "Contact_manifold.h"
#include "Physics_object.h"

#include <Vector_math.h>

#include <algorithm>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

//////////////////////////////////////////////////////////////
Contact_manifold::Contact_manifold( std::shared_ptr<Physics_object> a, std::shared_ptr<Physics_object> b )
    : m_object_a( a )
    , m_object_b( b )
{
}

//////////////////////////////////////////////////////////////
void Contact_manifold::prune_old_contacts()
{
    std::remove_if( m_contacts.begin(), m_contacts.end(), 
                    [this]( const Contact& c ){
                        Math::Point new_contact_a = m_object_a->coordinate_space().transform( c.local_point_a );
                        Math::Point new_contact_b = m_object_b->coordinate_space().transform( c.local_point_b );

                        // check to see if it's still penetrating
                        if (Math::dot_product( Math::Vector(c.normal), Math::to_vector(new_contact_b)-Math::to_vector(new_contact_a) ) >= 0.0f) {
                            return true;
                        }

                        const float THRESHOLD = 0.01f;
                        // if the point has moved too far from where it was originally recorded
                        // then we want to remove if from the manifold
                        if ((new_contact_a-c.contact_point_a).length_squared() > THRESHOLD) {
                            return true;
                        }
                        if ((new_contact_b-c.contact_point_b).length_squared() > THRESHOLD) {
                            return true;
                        }
                        return false;
                    }
                  );
}

//////////////////////////////////////////////////////////////
void Contact_manifold::insert( const std::vector<Contact>& contacts )
{
    // temporary
    m_contacts.clear();
    m_contacts = contacts;
}

}}
