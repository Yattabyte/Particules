#pragma once
#ifndef COLLISIONSYSTEM_HPP
#define COLLISIONSYSTEM_HPP

#include "collision.hpp"
#include "components.hpp"
#include "ecsWorld.hpp"

///////////////////////////////////////////////////////////////////////////
/// Use the shared mini namespace
using namespace mini;

///////////////////////////////////////////////////////////////////////////
/// \class  CollisionResolverSystem
/// \brief  Class is used to resolve collisions instances
///         A meta system of sorts.
class CollisionResolverSystem {
    public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Resolve collisions between every entity.
    /// \param  deltaTime   the amount of time since last frame.
    /// \param  world       the game world to resolve for.
    static void resolveCollisions(const double&, ecsWorld& world);
};

#endif // COLLISIONSYSTEM_HPP