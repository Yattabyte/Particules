#pragma once
#ifndef COMBUSTIONSYSTEM_HPP
#define COMBUSTIONSYSTEM_HPP

#include "components.hpp"
#include "ecsSystem.hpp"
#include "ecsWorld.hpp"
#include "quadTree.hpp"

///////////////////////////////////////////////////////////////////////////
/// Use the shared mini namespace
using namespace mini;

///////////////////////////////////////////////////////////////////////////
/// \class  ExplosionSystem
/// \brief  Class is used to combust explosive entities.
class CombustionSystem final : public ecsSystem {
    public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Construct a cleanup system.
    /// \param  gameWorld   reference to the engine's game world.
    CombustionSystem(
        ecsWorld& gameWorld, const QuadTree<ParticleComponent*>& quadTree);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Tick this system by deltaTime.
    /// \param	deltaTime	    the amount of time passed since last update.
    /// \param	components	    the components to update.
    void updateComponents(
        const double&,
        const std::vector<std::vector<ecsBaseComponent*>>& entityComponents)
        final;

    private:
    ecsWorld& m_gameWorld;
    const QuadTree<ParticleComponent*>& m_quadTree;
};

#endif // COMBUSTIONSYSTEM_HPP