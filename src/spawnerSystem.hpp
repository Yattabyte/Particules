#pragma once
#ifndef SPAWNERSYSTEM_HPP
#define SPAWNERSYSTEM_HPP

#include "components.hpp"
#include "ecsSystem.hpp"
#include "ecsWorld.hpp"

///////////////////////////////////////////////////////////////////////////
/// Use the shared mini namespace
using namespace mini;

/////////////////////////////////////////////////////////////////////////
/// \class  SpawnerSystem
/// \brief  System used to spawn new particles out faucets.
class SpawnerSystem final : public ecsSystem {
    public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Construct a spawner system.
    /// \param  gameWorld       reference to the engine's game world.
    /// \param  particleArray   structure identifying particles spatially.
    SpawnerSystem(ecsWorld& gameWorld,
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

#endif // SPAWNERSYSTEM_HPP