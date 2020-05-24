#pragma once
#ifndef COLLISIONFINDERSYSTEM_HPP
#define COLLISIONFINDERSYSTEM_HPP

#include "collision.hpp"
#include "components.hpp"
#include "ecsWorld.hpp"
#include "quadTree.hpp"

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
    /// \brief  Construct a collision finder system.
    CollisionFinderSystem() : m_quadTree(vec2(0), vec2(250.0F), 0) {}
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Find collisions between every entity.
    /// \param  deltaTime   the amount of time since last frame.
    /// \param  world       the game world to resolve for.
    void findCollisions(const double&, ecsWorld& world);

    QuadTree<ParticleComponent*> m_quadTree; ///< Partitions world.
};

#endif // COLLISIONFINDERSYSTEM_HPP