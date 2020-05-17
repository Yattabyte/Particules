#pragma once
#ifndef COLLISIONFINDERSYSTEM_HPP
#define COLLISIONFINDERSYSTEM_HPP

#include "collision.hpp"
#include "components.hpp"
#include "ecsWorld.hpp"

///////////////////////////////////////////////////////////////////////////
/// Use the shared mini namespace
using namespace mini;

///////////////////////////////////////////////////////////////////////////
/// \class  CollisionFinderSystem
/// \brief  Class is used to find collision instances.
///         A meta system of sorts.
class CollisionFinderSystem {
    public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Find collisions between every entity.
    /// \param  deltaTime   the amount of time since last frame.
    /// \param  world       the game world to resolve for.
    static void resolveCollisions(const double&, ecsWorld& world);
};

#endif // COLLISIONFINDERSYSTEM_HPP