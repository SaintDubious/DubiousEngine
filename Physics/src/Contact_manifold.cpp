#include "Contact_manifold.h"
#include "Physics_object.h"

#include <Vector_math.h>

#include <algorithm>
#include <iostream>
#include <tuple>

namespace Dubious {
namespace Physics {

Contact_manifold::Contact_manifold(Physics_object& a, Physics_object& b, float persistent_threshold,
                                   float movement_threshold)
    : m_object_a(a)
    , m_object_b(b)
    , m_persistent_threshold(persistent_threshold)
    , m_movement_threshold(movement_threshold)
{
}

void
Contact_manifold::prune_old_contacts()
{
    m_contacts.erase(
        std::remove_if(
            m_contacts.begin(), m_contacts.end(),
            [this](Contact& c) {
                Math::Point new_contact_a =
                    m_object_a.coordinate_space().transform(c.local_point_a);
                Math::Point new_contact_b =
                    m_object_b.coordinate_space().transform(c.local_point_b);

                // check to see if it's still penetrating
                float dot_penetrator =
                    Math::dot_product(Math::Vector(c.normal), Math::to_vector(new_contact_b) -
                                                                  Math::to_vector(new_contact_a));
                if (dot_penetrator > 0.0f) {
                    return true;
                }
                c.penetration_depth = dot_penetrator;

                // if the point has moved too far from where it was originally recorded
                // then we want to remove if from the manifold
                if ((new_contact_a - c.contact_point_a).length_squared() > m_movement_threshold) {
                    return true;
                }
                if ((new_contact_b - c.contact_point_b).length_squared() > m_movement_threshold) {
                    return true;
                }
                return false;
            }),
        m_contacts.end());
}

// How to find a manifold
// http://allenchou.net/2014/01/game-physics-stability-warm-starting/
// The general idea is to find up to 4 points that define the largest
// shape from the soup of points passed in. In general the algorithm is:
//   1. start with the point that has the deepest penetration
//   2. find the point farthest from it
//   3. find the point farthest from that line segment
//   4. find the point farthest from that triangle

// http://www.geometrictools.com/Documentation/DistancePointLine.pdf
float
Contact_manifold::distance_squared_to_line_segment(const Math::Point& a, const Math::Point& b,
                                                   const Math::Point& p) const
{
    Math::Vector direction = b - a;
    float        t         = dot_product(direction, p - a) / dot_product(direction, direction);
    if (t <= 0) {
        return (p - a).length_squared();
    }
    else if (t >= 1) {
        return (p - b).length_squared();
    }
    return (p - (a + (direction * t))).length_squared();
}

// Find out which segment of the triangle is closest to the point.
// We do this by comparing the cross product of the line segment
// formed by p and a, b, or c with the triangle normal. We can then tell
// if the point is on the "inside" or "outside of each triangle segment.
// If it's outside a given segment, then that segment is closest. If
// it's inside all segments then it's inside the triangle and we're
// not inetersted in it for the manifold.
// The winding direction of the triangle is not important, all we care
// about is if the cross product of the p segment is on the same side
// as the normal, we don't care if that side is + or -
std::tuple<bool, Math::Point, Math::Point>
Contact_manifold::closest_segment(const Math::Point& a, const Math::Point& b, const Math::Point& c,
                                  const Math::Point& p) const
{
    Math::Vector edge_a_b = b - a;
    Math::Vector edge_a_c = c - a;
    Math::Vector normal   = Math::cross_product(edge_a_b, edge_a_c);

    // Segment AP against AB
    Math::Vector edge_a_p = p - a;
    if (Math::dot_product(normal, Math::cross_product(edge_a_b, edge_a_p)) < 0) {
        return std::make_tuple(true, a, b);
    }
    // Segment AP against AC
    if (Math::dot_product(normal, Math::cross_product(edge_a_p, edge_a_c)) < 0) {
        return std::make_tuple(true, a, c);
    }
    // Segment BP against BC
    Math::Vector edge_b_p = p - b;
    Math::Vector edge_b_c = c - b;
    if (Math::dot_product(normal, Math::cross_product(edge_b_c, edge_b_p)) < 0) {
        return std::make_tuple(true, b, c);
    }

    // If the point is not outside all of the segments, then it must be
    // inside the triangle
    return std::make_tuple(false, a, b);
}

std::tuple<bool, float>
Contact_manifold::distance_squared_to_triangle(const Math::Point& a, const Math::Point& b,
                                               const Math::Point& c, const Math::Point& p) const
{
    float       intersects;
    Math::Point i1, i2;
    std::tie(intersects, i1, i2) = closest_segment(a, b, c, p);
    if (!intersects) {
        return std::make_tuple(false, 0.0f);
    }
    return std::make_tuple(true, distance_squared_to_line_segment(i1, i2, p));
}

void
Contact_manifold::cleanup_contacts(std::vector<Contact>& contacts)
{
    if (contacts.size() <= 4) {
        return;
    }
    std::vector<Contact_manifold::Contact> new_contacts(4);

    //
    // start with deepest contacts
    float  furthest = std::numeric_limits<float>::lowest();
    size_t furthest_index;
    for (size_t i = 0; i < contacts.size(); ++i) {
        if (contacts[i].penetration_depth > furthest) {
            furthest       = contacts[i].penetration_depth;
            furthest_index = i;
        }
    }
    std::swap(contacts.back(), contacts[furthest_index]);
    new_contacts[0] = contacts.back();
    contacts.pop_back();

    // find the one furthest away
    furthest = std::numeric_limits<float>::lowest();
    for (size_t i = 0; i < contacts.size(); ++i) {
        float distance =
            (new_contacts[0].contact_point_a - contacts[i].contact_point_a).length_squared();
        if (distance > furthest) {
            furthest       = distance;
            furthest_index = i;
        }
    }
    std::swap(contacts.back(), contacts[furthest_index]);
    new_contacts[1] = contacts.back();
    contacts.pop_back();

    // find the furthest from the line segment
    furthest = std::numeric_limits<float>::lowest();
    for (size_t i = 0; i < contacts.size(); ++i) {
        float distance = distance_squared_to_line_segment(new_contacts[0].contact_point_a,
                                                          new_contacts[1].contact_point_a,
                                                          contacts[i].contact_point_a);
        if (distance > furthest) {
            furthest       = distance;
            furthest_index = i;
        }
    }
    std::swap(contacts.back(), contacts[furthest_index]);
    new_contacts[2] = contacts.back();
    contacts.pop_back();

    // find the furthest from the triangle
    furthest = std::numeric_limits<float>::lowest();
    for (size_t i = 0; i < contacts.size(); ++i) {
        bool  valid;
        float distance;
        std::tie(valid, distance) = distance_squared_to_triangle(
            new_contacts[0].contact_point_a, new_contacts[1].contact_point_a,
            new_contacts[2].contact_point_a, contacts[i].contact_point_a);
        if (valid && distance > furthest) {
            furthest       = distance;
            furthest_index = i;
        }
    }
    if (furthest != std::numeric_limits<float>::lowest()) {
        new_contacts[3] = contacts[furthest_index];
    }

    contacts = new_contacts;
}

void
Contact_manifold::insert(const std::vector<Contact>& contacts)
{
    for (const auto& c : contacts) {
        bool add = true;
        for (auto& existing : m_contacts) {
            if ((c.contact_point_a - existing.contact_point_a).length_squared() <
                    m_persistent_threshold &&
                (c.contact_point_b - existing.contact_point_b).length_squared() <
                    m_persistent_threshold) {
                add = false;

                auto old                = existing;
                existing                = c;
                existing.normal_impulse = old.normal_impulse;

                break;
            }
        }
        if (add) {
            m_contacts.push_back(c);
        }
    }
    cleanup_contacts(m_contacts);
}

std::ostream&
operator<<(std::ostream& o, const Contact_manifold& c)
{
    for (const auto& contact : c.m_contacts) {
        o << "{\n\t" << contact.local_point_a << "\n\t" << contact.local_point_b << "\n}\n";
    }
    return o;
}

}  // namespace Physics
}  // namespace Dubious
