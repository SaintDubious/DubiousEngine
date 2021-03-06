#ifndef INCLUDED_PHYSICS_CONTACT_MANIFOLD
#define INCLUDED_PHYSICS_CONTACT_MANIFOLD

#include <Point.h>
#include <Unit_vector.h>
#include <Coordinate_space.h>

#include <memory>
#include <vector>

namespace Physics_test {
class Contact_manifold_test;
}

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
    Contact_manifold(Physics_object& a, Physics_object& b, float persistent_threshold,
                     float movement_threshold);

    /// @brief Contact information
    ///
    /// The result of a collision will be a vector of these.
    /// Contains information relevant to the contact
    struct Contact {
        Math::Point       contact_point_a;
        Math::Local_point local_point_a;
        Math::Point       contact_point_b;
        Math::Local_point local_point_b;
        Math::Unit_vector normal;
        Math::Unit_vector tangent1;
        Math::Unit_vector tangent2;
        float             penetration_depth = 0;
        float             normal_impulse    = 0;
        float             tangent1_impulse  = 0;
        float             tangent2_impulse  = 0;
    };

    /// @brief Prunes old contact points
    ///
    /// The manifold contains contact points from the previous time step.
    /// Some of these may have drifted out of contact, or they may have
    /// moved so far that we no longer want them in our manifold.
    void prune_old_contacts();

    /// @brief Inserts contacts into the manifold;
    ///
    /// Does the job of deciding if these contacts already exist
    /// and if so, maybe use the older ones? Or newer ones?
    void insert(const std::vector<Contact>& contacts);

    /// @brief Scale the contact impulses
    ///
    /// Basically iterate through the Contacts and scale the normal_impulse by this supplied factor.
    /// This is used primarily by the main loop to scale the impulse for warm starting (or reset to
    /// 0 if warm starting is disabled).
    /// @param scale - [in] the amount to scale by
    void scale_contact_impulses(float scale);

    /// @brief contacts accessors
    std::vector<Contact>&       contacts() { return m_contacts; }
    const std::vector<Contact>& contacts() const { return m_contacts; }

    Physics_object& object_a() { return m_object_a; }
    Physics_object& object_b() { return m_object_b; }

    Math::Vector& a_delta_velocity() { return m_a_delta_velocity; }
    Math::Vector& a_delta_angular_velocity() { return m_a_delta_angular_velocity; }
    Math::Vector& b_delta_velocity() { return m_b_delta_velocity; }
    Math::Vector& b_delta_angular_velocity() { return m_b_delta_angular_velocity; }

private:
    friend class Physics_test::Contact_manifold_test;
    friend std::ostream& operator<<(std::ostream& o, const Contact_manifold&);

    void  cleanup_contacts(std::vector<Contact>& contacts);
    float distance_squared_to_line_segment(const Math::Point& a, const Math::Point& b,
                                           const Math::Point& p) const;
    std::tuple<bool, float> distance_squared_to_triangle(const Math::Point& a, const Math::Point& b,
                                                         const Math::Point& c,
                                                         const Math::Point& p) const;
    std::tuple<bool, Math::Point, Math::Point> closest_segment(const Math::Point& a,
                                                               const Math::Point& b,
                                                               const Math::Point& c,
                                                               const Math::Point& p) const;

    Physics_object&      m_object_a;
    Physics_object&      m_object_b;
    std::vector<Contact> m_contacts;
    const float          m_movement_threshold   = 0.05f;
    const float          m_persistent_threshold = 0.05f;

    Math::Vector m_a_delta_velocity;
    Math::Vector m_a_delta_angular_velocity;
    Math::Vector m_b_delta_velocity;
    Math::Vector m_b_delta_angular_velocity;
};

std::ostream& operator<<(std::ostream& o, const Contact_manifold& c);

}  // namespace Physics
}  // namespace Dubious

#endif
