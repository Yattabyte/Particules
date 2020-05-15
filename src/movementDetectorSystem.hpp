#pragma once
#ifndef MOVEMENTDETECTORSYSTEM_HPP
#define MOVEMENTDETECTORSYSTEM_HPP

#include "components.hpp"
#include "ecsSystem.hpp"
#include "ecsWorld.hpp"

///////////////////////////////////////////////////////////////////////////
/// Use the shared mini namespace
using namespace mini;

/////////////////////////////////////////////////////////////////////////
/// \class  MovementDetectorSystem
/// \brief  System used to flag entities as moving or stationary.
class MovementDetectorSystem final : public ecsSystem {
    public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Construct a movement detector system.
    /// \param  gameWorld   reference to the engine's game world.
    MovementDetectorSystem(ecsWorld& gameWorld);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Tick this system by deltaTime.
    /// \param	deltaTime	    the amount of time passed since last update.
    /// \param	components	    the components to update.
    void updateComponents(
        const double& deltaTime,
        const std::vector<std::vector<ecsBaseComponent*>>& entityComponents)
        final;

    private:
    ecsWorld& m_gameWorld;
};

#endif // MOVEMENTDETECTORSYSTEM_HPP