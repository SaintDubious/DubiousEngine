#ifndef INCLUDED_PHYSICS_CONSTRAINT_STRATEGY_H
#define INCLUDED_PHYSICS_CONSTRAINT_STRATEGY_H

#include "Contact_manifold.h"

#include <map>

namespace Dubious {
namespace Physics {

class Physics_object;

class Constraint_strategy {
public:
    Constraint_strategy( const Constraint_strategy& ) = delete;
    Constraint_strategy& operator=( const Constraint_strategy& ) = delete;

    virtual void warm_start( std::map<std::tuple<Physics_object*,Physics_object*>, Contact_manifold> &manifolds ) = 0;
    virtual void solve( int iterations, std::map<std::tuple<Physics_object*,Physics_object*>, Contact_manifold> &manifolds ) = 0;

    ~Constraint_strategy() = default;
protected:
    Constraint_strategy() = default;
};

}}

#endif
