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
    float MaxDot = std::numeric_limits<float>::lowest();
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
bool NearestSimplex2( std::vector<Math::Vector>& Simplex, Math::Vector& Direction )
{
    const Math::Vector& A = Simplex[1];
    const Math::Vector& B = Simplex[0];
    const Math::Vector Origin;

    const Math::Vector& AB = B - A;
    const Math::Vector& AO = Origin - A;
    Direction = Math::CrossProduct( Math::CrossProduct( AB, AO ), AB );
    if (Direction == Math::Vector()) {
        // If the cross product is (0,0,0) then the origin lies on this
        // line. No need to go any further.
        return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////
bool NearestSimplex3( std::vector<Math::Vector>& Simplex, Math::Vector& Direction )
{
    // Winding order is important for our triangle. I've tried my 
    // best to look at this solution with both a clockwise and
    // counter-clockwise triangle and verify that the triangle
    // that results as the base of the tetrahedron will be correctly
    // wound counter-clockwise with respect to the incoming point.
    // http://hacktank.net/blog/?p=93
    const Math::Vector& A = Simplex[2];
    const Math::Vector& B = Simplex[1];
    const Math::Vector& C = Simplex[0];
    const Math::Vector Origin;

    const Math::Vector& AB = B - A;
    const Math::Vector& AC = C - A;
    const Math::Vector& AO = Origin - A;
    const Math::Vector& ABxAC = Math::CrossProduct( AB, AC );

    const Math::Vector& ABPerp = Math::CrossProduct( AB, ABxAC );
    if (Math::DotProduct( AO, ABPerp ) > 0) {
        // The point lies outside of the triangle on the AB side
        Direction = ABPerp;
        Simplex[0] = Simplex[1];
        Simplex[1] = Simplex[2];
        Simplex.pop_back();
        return false;
    }
    const Math::Vector& ACPerp = Math::CrossProduct( ABxAC, AC );
    if (Math::DotProduct( AO, ACPerp ) > 0) {
        // The point lies outside of the triangle on the AC side
        Direction = ACPerp;
        Simplex[1] = Simplex[2];
        Simplex.pop_back();
        return false;
    }

    // The point lies inside the triangle
    // Now the question is if it's above or below
    float SideCheck = Math::DotProduct( ABxAC, AO );
    if (Math::Equals( SideCheck, 0 )) {
        // If the side check finds that the point lies 
        // within this triangle then there's no need
        // to continue
        return true;
    }
    else if ( SideCheck > 0) {
        // This direction is above the triangle (ie on the same side
        // as the triangle normal). We keep everything the same and
        // use the direction as the triangle normal
        Direction = ABxAC;
    }
    else {
        // This is below the triangle. We want to make sure that in
        // the tetrahedron case the new point is on the "top" of the
        // triangle, so we will reverse the winding order. 
        Math::Vector Hold = Simplex[0];
        Simplex[0] = Simplex[1];
        Simplex[1] = Hold;
        Direction = ABxAC*-1;
    }

    return false;
}

//////////////////////////////////////////////////////////////
bool NearestSimplex4( std::vector<Math::Vector>& Simplex, Math::Vector& Direction )
{
    // The winding order of the tetrahedron is important. Point A is on the
    // "top" of the triangle defined by points B, C, and D. From A's point
    // of view, the triangle is wound counter-clockwise
    const Math::Vector& A = Simplex[3];
    const Math::Vector& B = Simplex[2];
    const Math::Vector& C = Simplex[1];
    const Math::Vector& D = Simplex[0];
    const Math::Vector Origin;

    const Math::Vector& AB = B-A;
    const Math::Vector& AC = C-A;
    const Math::Vector& AD = D-A;
    const Math::Vector& AO = Origin-A;

    const Math::Vector& ABxAC = Math::CrossProduct( AB, AC );
    if (Math::DotProduct( ABxAC, AO ) > 0) {
        Direction = ABxAC;
        Simplex[0] = Simplex[1];
        Simplex[1] = Simplex[2];
        Simplex[2] = Simplex[3];
        Simplex.pop_back();
        return false;
    }
    const Math::Vector& ACxAD = Math::CrossProduct( AC, AD );
    if (Math::DotProduct( ACxAD, AO ) > 0) {
        Direction = ACxAD;
        Simplex[2] = Simplex[3];
        Simplex.pop_back();
        return false;
    }
    const Math::Vector& ADxAB = Math::CrossProduct( AD, AB );
    if (Math::DotProduct( ADxAB, AO ) > 0) {
        Direction = ADxAB;
        Simplex[1] = Simplex[2];
        Simplex[2] = Simplex[3];
        Simplex.pop_back();
        return false;
    }

    // If the AO point is "behind" all 3 faces then it must
    // be inside the tetrahedron
    return true;
}

//////////////////////////////////////////////////////////////
bool NearestSimplex( std::vector<Math::Vector>& Simplex, Math::Vector& Direction )
{
    switch (Simplex.size()) {
    case 2:
        return NearestSimplex2( Simplex, Direction ); 
    case 3:
        return NearestSimplex3( Simplex, Direction ); 
    case 4:
        return NearestSimplex4( Simplex, Direction ); 
    default:
        throw std::runtime_error( "Received Simplex of incorrect size" );
    }
}

//////////////////////////////////////////////////////////////
bool CollisionSolver::Intersection( const PhysicsObject& A, const PhysicsObject& B )
{
    A.CacheCollisionVectors();
    B.CacheCollisionVectors();

    Math::Vector Direction( 1, 0, 0 );
    Math::Vector SimplexPoint = Support( A, Direction ) - Support( B, Direction*-1 );
    std::vector<Math::Vector> Simplex;
    Simplex.push_back( SimplexPoint );
    Direction = Math::Vector() - SimplexPoint;
    while (true) {
        SimplexPoint = Support( A, Direction ) - Support( B, Direction*-1 );
        // If this next check is < 0 then touching will be considered
        // a collision. If it's <= 0 then thouching will not be a collision
        // .... I think. Not very well tested
        if (Math::DotProduct( SimplexPoint, Direction ) <= 0) {
            return false;
        }
        Simplex.push_back( SimplexPoint );
        if (NearestSimplex( Simplex, Direction )) {
            return true;
        }
    }
    
    return false;
}

}}

