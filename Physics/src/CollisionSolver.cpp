#include "CollisionSolver.h"
#include "PhysicsObject.h"

#include <VectorMath.h>

#include <vector>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

//////////////////////////////////////////////////////////////
Math::Vector Support( const PhysicsObject& Object, const Math::Vector& Direction )
{
    Math::Vector Result;
    float MaxDot = std::numeric_limits<float>::min();
    for (const Math::Vector& V : Object.CachedCollisionVectors()) {
        float Dot = Math::DotProduct( V, Direction );
        if (Dot > MaxDot) {
            MaxDot = Dot;
            Result = V;
        }
    }
    return Result;
}

//////////////////////////////////////////////////////////////
bool NearestSimplex( std::vector<Math::Vector>& Simplex, Math::Vector& Direction )
{
    return false;
}

//////////////////////////////////////////////////////////////
bool CollisionSolver::Intersection( const PhysicsObject& A, const PhysicsObject& B )
{
    A.CacheCollisionVectors();
    B.CacheCollisionVectors();

    Math::Vector P = Support( A, Math::Vector( 1, 0, 0 ) ) - Support( B, Math::Vector( -1, 0, 0 ) );
    std::vector<Math::Vector> Simplex;
    Simplex.push_back( P );
    Math::Vector Direction = P*-1;
    while (true) {
        P = Support( A, Direction ) - Support( B, Direction*-1 );
        if (Math::DotProduct( P, Direction ) < 0) {
            return false;
        }
        Simplex.push_back( P );
        if (NearestSimplex( Simplex, Direction )) {
            return true;
        }
    }
    
    return false;
}

}}

