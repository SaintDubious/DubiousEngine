#include "Constraint_strategy_multi_threaded.h"

#include <future>

namespace Dubious {
namespace Physics {

Constraint_strategy_multi_threaded::Constraint_strategy_multi_threaded(float time_step, float beta,
                                                                       float cor, float slop)
    : m_constraint_solver(time_step, beta, cor, slop)
{
}

Constraint_strategy_multi_threaded::~Constraint_strategy_multi_threaded()
{
}

void
Constraint_strategy_multi_threaded::warm_start(
    std::map<std::tuple<Physics_object*, Physics_object*>, Contact_manifold>& manifolds)
{
    std::vector<std::future<void>> futures;
    std::vector<Manifold_info>     info;
    const int                      MANIFOLD_PER_THREAD = 750;
    for (auto& manifold : manifolds) {
        info.push_back(Manifold_info());
        info.back().a        = std::get<0>(manifold.first);
        info.back().b        = std::get<1>(manifold.first);
        info.back().manifold = &manifold.second;
        if (info.size() >= MANIFOLD_PER_THREAD) {
            futures.push_back(std::async(std::launch::async,
                                         &Constraint_strategy_multi_threaded::warm_start_internal,
                                         this, std::move(info)));
            info.clear();
        }
    }
    futures.push_back(std::async(std::launch::async,
                                 &Constraint_strategy_multi_threaded::warm_start_internal, this,
                                 std::move(info)));

    for (auto& f : futures) {
        f.get();
    }
}

void
Constraint_strategy_multi_threaded::solve(
    int                                                                       iterations,
    std::map<std::tuple<Physics_object*, Physics_object*>, Contact_manifold>& manifolds)
{
    for (int i = 0; i < iterations; ++i) {
        std::vector<std::future<void>> futures;
        std::vector<Manifold_info>     info;
        const int                      MANIFOLD_PER_THREAD = 750;
        for (auto& manifold : manifolds) {
            info.push_back(Manifold_info());
            info.back().a        = std::get<0>(manifold.first);
            info.back().b        = std::get<1>(manifold.first);
            info.back().manifold = &manifold.second;
            if (info.size() >= MANIFOLD_PER_THREAD) {
                futures.push_back(std::async(std::launch::async,
                                             &Constraint_strategy_multi_threaded::solve_internal,
                                             this, std::move(info)));
                info.clear();
            }
        }
        futures.push_back(std::async(std::launch::async,
                                     &Constraint_strategy_multi_threaded::solve_internal, this,
                                     std::move(info)));
        for (auto& f : futures) {
            f.get();
        }
    }
}

void
Constraint_strategy_multi_threaded::warm_start_internal(std::vector<Manifold_info>&& manifolds)
{
    for (auto& m : manifolds) {
        m_constraint_solver.warm_start(*m.a, *m.b, *m.manifold);
    }
}

void
Constraint_strategy_multi_threaded::solve_internal(std::vector<Manifold_info>&& manifolds)
{
    for (auto& m : manifolds) {
        m_constraint_solver.solve(*m.a, *m.b, *m.manifold);
    }
}

}  // namespace Physics
}  // namespace Dubious
