#include "Contact_manifold.h"

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
void Contact_manifold::insert( const std::vector<Contact>& contacts )
{
    // temporary
    m_contacts.clear();
    m_contacts = contacts;
}

}}
