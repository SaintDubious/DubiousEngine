#include "PhysicsObject.h"
#include "PhysicsModel.h"
#include "Vector.h"

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

//////////////////////////////////////////////////////////////
PhysicsObject::PhysicsObject( PhysicsModelPtr pModel )
    : m_pModel( pModel )
{
}

//////////////////////////////////////////////////////////////
namespace {
void CopyModelToVectors( const PhysicsModel& Model, Math::CoordinateSpace Coords, Math::VectorVector& Vectors )
{
    Math::LocalVector Offset = Model.Offset() - Math::LocalPoint();
    for (const Math::LocalPoint P : Model.Points()) {
        Math::Vector NewVect = Coords.Transform( Offset + (P-Math::LocalPoint()) );
        NewVect = NewVect + (Coords.Position() - Math::Point());
        Vectors.push_back( NewVect );
    }
    Coords.Translate( Offset );
    for (const PhysicsModelPtr pKid : Model.Kids()) {
        CopyModelToVectors( *pKid, Coords, Vectors );
    }
}
}

//////////////////////////////////////////////////////////////
void PhysicsObject::CacheCollisionVectors() const
{
    if (m_CoordinateSpace == m_CachedCoordinateSpace && !m_CachedCollisionVectors.empty()) {
        return;
    }
    m_CachedCollisionVectors.clear();
    CopyModelToVectors( *m_pModel, m_CoordinateSpace, m_CachedCollisionVectors );
    m_CachedCoordinateSpace = m_CoordinateSpace;
}

}}
