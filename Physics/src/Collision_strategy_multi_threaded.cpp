#include "Collision_strategy_multi_threaded.h"
#include "Physics_object.h"
#include "Physics_model.h"
#include "Contact_manifold.h"

#include <set>
#include <future>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

//////////////////////////////////////////////////////////////
Collision_strategy_multi_threaded::Collision_strategy_multi_threaded( float manifold_persistent_threshold, unsigned int workgroup_size )
    : m_manifold_persistent_threshold( manifold_persistent_threshold )
    , m_workgroup_size( workgroup_size )
{
}

//////////////////////////////////////////////////////////////
void Collision_strategy_multi_threaded::find_contacts( const std::vector<std::shared_ptr<Physics_object>>& objects,
                                               std::map<Physics_object_pair, Contact_manifold>& manifolds )
{
    std::vector<std::future<std::set<Physics_object_pair>>> local_pairs;

    for (size_t i=0; i<objects.size(); i+=m_workgroup_size) {
        unsigned int length = m_workgroup_size;
        if (i+length > objects.size()) {
            length = objects.size() - i;
        }
        local_pairs.push_back( std::async( std::launch::async, 
                                           &Collision_strategy_multi_threaded::solve_inner, this, i, length, 
                                           std::cref(objects), std::ref(manifolds) ) );
    }
    for (size_t i=0; i<objects.size(); i+=m_workgroup_size) {
        for (size_t j=i+m_workgroup_size; j<objects.size(); j+=m_workgroup_size) {
            unsigned int length = m_workgroup_size;
            if (j+length > objects.size()) {
                length = objects.size() - j;
            }
            local_pairs.push_back( std::async( std::launch::async, 
                                               &Collision_strategy_multi_threaded::solve_outer, this, i, m_workgroup_size, j, length, 
                                               std::cref(objects), std::ref(manifolds) ) );
        }
    }

    // collect the results
    std::set<Physics_object_pair> new_pairs;
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
std::set<Collision_strategy::Physics_object_pair> Collision_strategy_multi_threaded::solve_inner( size_t start, size_t length,
                                                  const std::vector<std::shared_ptr<Physics_object>>& objects,
                                                  std::map<Physics_object_pair, Contact_manifold>& manifolds )
{
    std::set<Physics_object_pair> new_pairs;
    for (size_t i=start; i<start+length; ++i) {
        auto a = objects[i].get();
        for (size_t j=i+1; j<start+length; ++j) {
            auto b = objects[j].get();
            if (!m_collision_solver.broad_phase_intersection( *a, *b )) {
                continue;
            }
            std::vector<Contact_manifold::Contact> contacts;
            if (m_collision_solver.intersection( *a, *b, contacts )) {
                auto object_pair = std::make_tuple(a,b);
                auto contact_manifold = manifolds.find( object_pair );
                if (contact_manifold == manifolds.end()) {
                    std::unique_lock<std::mutex> lock( m_manifolds_mutex );
                    contact_manifold = manifolds.insert( std::make_pair(object_pair, 
                        Contact_manifold( *a, *b, m_manifold_persistent_threshold )) ).first;
                }
                contact_manifold->second.prune_old_contacts();
                contact_manifold->second.insert( contacts );
                new_pairs.insert( object_pair );
            }
        }
    }
    return new_pairs;
}

//////////////////////////////////////////////////////////////
#pragma warning( disable : 4503 ) // decorated name length exceeded, name was truncated
std::set<Collision_strategy::Physics_object_pair> Collision_strategy_multi_threaded::solve_outer( size_t a_start, size_t a_length, size_t b_start, size_t b_length,
                                                  const std::vector<std::shared_ptr<Physics_object>>& objects,
                                                  std::map<Physics_object_pair, Contact_manifold>& manifolds )
{
    std::set<Physics_object_pair> new_pairs;
    for (size_t i=a_start; i<a_start+a_length; ++i) {
        auto a = objects[i].get();
        for (size_t j=b_start; j<b_start+b_length; ++j) {
            auto b = objects[j].get();
            if (!m_collision_solver.broad_phase_intersection( *a, *b )) {
                continue;
            }
            std::vector<Contact_manifold::Contact> contacts;
            if (m_collision_solver.intersection( *a, *b, contacts )) {
                auto object_pair = std::make_tuple(a,b);
                auto contact_manifold = manifolds.find( object_pair );
                if (contact_manifold == manifolds.end()) {
                    std::unique_lock<std::mutex> lock( m_manifolds_mutex );
                    contact_manifold = manifolds.insert( std::make_pair(object_pair, 
                        Contact_manifold( *a, *b, m_manifold_persistent_threshold )) ).first;
                }
                contact_manifold->second.prune_old_contacts();
                contact_manifold->second.insert( contacts );
                new_pairs.insert( object_pair );
            }
        }
    }
    return new_pairs;
}

}
}
