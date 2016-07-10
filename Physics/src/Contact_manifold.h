#ifndef INCLUDED_PHYSICS_CONTACT_MANIFOLD
#define INCLUDED_PHYSICS_CONTACT_MANIFOLD

#include <Point.h>
#include <Unit_vector.h>

#include <memory>
#include <vector>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

class Physics_object;

/// @brief Contains persistent contact information
///
/// At each time slice a single contact between 2 intersecting
/// objects can be found. Over a number of time slices a group 
/// of these contacts points are built up to create a contact
/// manifold. This manifold class handles managing whether or
/// not new contacts should be added, when old contacts should
/// be removed, etc. Check out "Warm Starting"
/// http://allenchou.net/2014/01/game-physics-stability-warm-starting/
class Contact_manifold {
public:

    Contact_manifold( std::shared_ptr<Physics_object> a, std::shared_ptr<Physics_object> b );

    /// @brief Contact information
    ///
    /// The result of a collision will be a vector of these. 
    /// Contains information relevant to the contact
    struct Contact {
        Math::Point     contact_point_a;
        Math::Local_point local_point_a;
        Math::Point     contact_point_b;
        Math::Local_point local_point_b;
        Math::Unit_vector normal;
        float           penetration_depth;
    };

    /// @brief Prunes old contact points
    ///
    /// The manifold contains contact points from the previous time step.
    /// Some of these may have drifted out of contact, or they may have
    /// moved so far that we no longer want them in our manifold.
    void                prune_old_contacts();

    /// @brief Inserts contacts into the manifold;
    ///
    /// Does the job of deciding if these contacts already exist
    /// and if so, maybe use the older ones? Or newer ones?
    void                insert( const std::vector<Contact>& contacts );

    const std::vector<Contact>& contacts() const { return m_contacts; }

    float&              normal_impulse_sum() { return m_normal_impulse_sum; }
private:
    std::shared_ptr<Physics_object> m_object_a;
    std::shared_ptr<Physics_object> m_object_b;
    std::vector<Contact> m_contacts;
    float               m_normal_impulse_sum = 0;
};

}}

#endif

