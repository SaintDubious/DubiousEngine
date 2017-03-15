#include "Collision_strategy_multi_threaded.h"
#include "Physics_object.h"
#include "Contact_manifold.h"

#include <set>
#include <future>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

Collision_strategy_multi_threaded::Collision_strategy_multi_threaded( float manifold_persistent_threshold, unsigned int collisions_per_thread )
    : m_manifold_persistent_threshold( manifold_persistent_threshold )
    , m_collisions_per_thread( collisions_per_thread )
{
}

void Collision_strategy_multi_threaded::find_contacts( const std::vector<std::shared_ptr<Physics_object>>& objects,
                                               std::map<Physics_object_pair, Contact_manifold>& manifolds )
{
    std::set<Physics_object_pair> new_pairs;
    std::vector<Physics_object_pair> inputs;
    std::vector<std::future<std::set<Physics_object_pair>>> local_pairs;
    for (size_t i=0; i<objects.size(); ++i) {
        auto a = objects[i];
        for (size_t j=i+1; j<objects.size(); ++j) {
            auto b = objects[j];
            if (!m_collision_solver.broad_phase_intersection( *a, *b )) {
                continue;
            }
            inputs.push_back( std::make_tuple( a, b ) );
            if (inputs.size() > m_collisions_per_thread) {
                local_pairs.push_back( std::async( std::launch::async, &Collision_strategy_multi_threaded::solve_collisions, this, std::move(inputs), std::ref(manifolds) ) );
                inputs.clear();              
            }
        }
    }
    // and the rest...
    local_pairs.push_back( std::async( std::launch::async, &Collision_strategy_multi_threaded::solve_collisions, this, std::move(inputs), std::ref(manifolds) ) );
    
    // collect the results
    for (auto& results : local_pairs) {
        const auto& result_set = results.get();
        new_pairs.insert( result_set.begin(), result_set.end() );
    }
    // remove any stale contacts
    for (auto iter=manifolds.begin(), end=manifolds.end(); iter!=end;) {
        if (new_pairs.find(iter->first) == new_pairs.end()) {
            manifolds.erase( iter++ );
        }
        else {
            ++iter;
        }
    }
}

//////////////////////////////////////////////////////////////
#pragma warning( disable : 4503 ) // decorated name length exceeded, name was truncated
std::set<Collision_strategy::Physics_object_pair> Collision_strategy_multi_threaded::solve_collisions( std::vector<Physics_object_pair>&& inputs,
                                        std::map<Physics_object_pair, Contact_manifold>& manifolds )
{
    std::set<Physics_object_pair> new_pairs;
    for (const auto& object_tuple : inputs) {
        std::vector<Contact_manifold::Contact> contacts;
        if (m_collision_solver.intersection( *std::get<0>(object_tuple), *std::get<1>(object_tuple), contacts )) {
            auto contact_manifold = manifolds.find( object_tuple );
            if (contact_manifold == manifolds.end()) {
                std::unique_lock<std::mutex> lock( m_manifolds_mutex );
                contact_manifold = manifolds.insert( std::make_pair(object_tuple, 
                    Contact_manifold( std::get<0>(object_tuple), std::get<1>(object_tuple), m_manifold_persistent_threshold )) ).first;
            }
            contact_manifold->second.prune_old_contacts();
            contact_manifold->second.insert( contacts );
            new_pairs.insert( object_tuple );
        }
    }
    return new_pairs;
}

}
}
