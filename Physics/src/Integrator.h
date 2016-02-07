#ifndef INCLUDED_PHYSICS_INTEGRATOR
#define INCLUDED_PHYSICS_INTEGRATOR

#include <list>
#include <memory>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

class Physics_object;

class Integrator {
public:
    Integrator( float step_size );

    void                update( std::list<std::shared_ptr<Physics_object>>& objects, float elapsed ); 

private:
    float               m_step_size;
    float               m_elapsed = 0.0f;
};

}}

#endif
