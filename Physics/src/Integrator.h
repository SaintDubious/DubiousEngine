#ifndef INCLUDED_PHYSICS_INTEGRATOR
#define INCLUDED_PHYSICS_INTEGRATOR

#include <list>
#include <memory>

namespace Physics_test { class Integrator_test; }

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

class Physics_object;

// Perfrom RK4 integration.  See:
// http://gafferongames.com/game-physics/integration-basics/
class Integrator {
public:
    Integrator( float step_size );

    void                update( std::list<std::shared_ptr<Physics_object>>& objects, float elapsed ); 

private:
    friend class Physics_test::Integrator_test;

    void                integrate_linear( Physics_object& physics_object, float dt ) const;
    void                integrate_angular( Physics_object& physics_object, float dt ) const;

    float               m_step_size;
    float               m_elapsed = 0.0f;
};

}}

#endif
