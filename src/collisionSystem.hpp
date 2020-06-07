#pragma once
#ifndef CollisionSystem_HPP
#define CollisionSystem_HPP

#include "collision.hpp"
#include "components.hpp"
#include "ecsWorld.hpp"
#include "quadTree.hpp"
#include <vector>

///////////////////////////////////////////////////////////////////////////
/// Use the shared mini namespace
using namespace mini;

///////////////////////////////////////////////////////////////////////////
/// \class  CollisionSystem
/// \brief  Class is used to apply physics.
///         A meta system of sorts.
class CollisionSystem final : public ecsSystem {
    public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Construct a collision finder system.
    /// \param  gameWorld       reference to the engine's game world.
    /// \param  particleArray   structure identifying particles spatially.
    explicit CollisionSystem(
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

#endif // CollisionSystem_HPP