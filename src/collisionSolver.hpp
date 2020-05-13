#pragma once
#ifndef COLLISIONSYSTEM_HPP
#define COLLISIONSYSTEM_HPP

#include "collision.hpp"
#include "components.hpp"
#include "ecsWorld.hpp"

constexpr float tolerance = 0.00001F;

///////////////////////////////////////////////////////////////////////////
/// Use the shared mini namespace
using namespace mini;

///////////////////////////////////////////////////////////////////////////
/// \class  CollisionSolver
/// \brief  Class is used to solve collisions for physics entities.
///         A meta system of sorts.
class CollisionSolver {
    public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Resolve collisions between every entity.
    /// \param  deltaTime   the amount of time since last frame.
    /// \param  world       the game world to resolve for.
    void resolveCollisions(const double&, ecsWorld& world);
};

#endif // COLLISIONSYSTEM_HPP