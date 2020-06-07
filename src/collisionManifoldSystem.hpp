#pragma once
#ifndef COLLISIONSYSTEM_HPP
#define COLLISIONSYSTEM_HPP

#include "collision.hpp"
#include "components.hpp"
#include "ecsWorld.hpp"
#include <vector>

///////////////////////////////////////////////////////////////////////////
/// Use the shared mini namespace
using namespace mini;

///////////////////////////////////////////////////////////////////////////
/// \class  CollisionManifoldSystem
/// \brief  Class is used to resolve collisions instances
class CollisionManifoldSystem final : public ecsSystem {
    public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Construct a collision manifold system.
    /// \param  gameWorld       reference to the engine's game world.
    /// \param  particleArray   structure identifying particles spatially.
    CollisionManifoldSystem(
        ecsWorld& gameWorld,
        std::shared_ptr<ParticleComponent* [513][513]>& particleArray);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Tick this system by deltaTime.
    /// \param	deltaTime	    the amount of time passed since last update.
    /// \param	components	    the components to update.
    void updateComponents(
        const double&,
        const std::vector<std::vector<ecsBaseComponent*>>& entityComponents)
        final;

    private:
    ///////////////////////////////////////////////////////////////////////////
    /// Private Members
    ecsWorld& m_gameWorld;
    std::shared_ptr<ParticleComponent* [513][513]>& m_particleArray;
};

#endif // COLLISIONSYSTEM_HPP