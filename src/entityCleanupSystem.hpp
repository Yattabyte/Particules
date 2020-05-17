#pragma once
#ifndef ENTITYCLEANUPSYSTEM_HPP
#define ENTITYCLEANUPSYSTEM_HPP

#include "components.hpp"
#include "ecsSystem.hpp"
#include "ecsWorld.hpp"

///////////////////////////////////////////////////////////////////////////
/// Use the shared mini namespace
using namespace mini;

/////////////////////////////////////////////////////////////////////////
/// \class  EntityCleanupSystem
/// \brief  System used to delete out-of-bound entities.
class EntityCleanupSystem final : public ecsSystem {
    public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Construct a cleanup system.
    /// \param  gameWorld   reference to the engine's game world.
    explicit EntityCleanupSystem(ecsWorld& gameWorld);

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
};

#endif // ENTITYCLEANUPSYSTEM_HPP