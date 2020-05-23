#pragma once
#ifndef IGNITIONSYSTEM_HPP
#define IGNITIONSYSTEM_HPP

#include "components.hpp"
#include "ecsSystem.hpp"
#include "ecsWorld.hpp"

///////////////////////////////////////////////////////////////////////////
/// Use the shared mini namespace
using namespace mini;

///////////////////////////////////////////////////////////////////////////
/// \class  IgnitionSystem
/// \brief  Class is used to ignite flammable particles
class IgnitionSystem final : public ecsSystem {
    public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Construct a cleanup system.
    /// \param  gameWorld   reference to the engine's game world.
    IgnitionSystem(ecsWorld& gameWorld);

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

#endif // IGNITIONSYSTEM_HPP