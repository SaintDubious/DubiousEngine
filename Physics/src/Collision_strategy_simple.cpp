#include "Collision_strategy_simple.h"
#include "Physics_object.h"
#include "Contact_manifold.h"

#include <set>

namespace Dubious {
namespace Physics {

Collision_strategy_simple::Collision_strategy_simple(float manifold_persistent_threshold,
                                                     float manifold_movement_threshold)
    : m_manifold_persistent_threshold(manifold_persistent_threshold)
    , m_manifold_movement_threshold(manifold_movement_threshold)
{
}

void
Collision_strategy_simple::find_contacts(
    const std::vector<std::shared_ptr<Physics_object>>& objects,
    std::map<Physics_object_pair, Contact_manifold>&    manifolds)
{
    std::set<Physics_object_pair> new_pairs;
    for (size_t i = 0; i < objects.size(); ++i) {
        auto a = objects[i].get();
        for (size_t j = i + 1; j < objects.size(); ++j) {
            auto b = objects[j].get();
            if (!m_collision_solver.broad_phase_intersection(*a, *b)) {
                continue;
            }
            std::vector<Contact_manifold::Contact> contacts;
            if (m_collision_solver.intersection(*a, *b, contacts)) {
                auto object_pair      = std::make_tuple(a, b);
                auto contact_manifold = manifolds.find(object_pair);
                if (contact_manifold == manifolds.end()) {
                    contact_manifold =
                        manifolds
                            .insert(std::make_pair(
                                object_pair,
                                Contact_manifold(*a, *b, m_manifold_persistent_threshold,
                                                 m_manifold_movement_threshold)))
                            .first;
                }
                contact_manifold->second.prune_old_contacts();
                contact_manifold->second.insert(contacts);
                new_pairs.insert(object_pair);
            }
        }
    }
    // remove any stale contacts
    for (auto iter = manifolds.begin(), end = manifolds.end(); iter != end;) {
        if (new_pairs.find(iter->first) == new_pairs.end()) {
            manifolds.erase(iter++);
        }
        else {
            ++iter;
        }
    }
}

}  // namespace Physics
}  // namespace Dubious
