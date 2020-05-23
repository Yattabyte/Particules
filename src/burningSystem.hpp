#pragma once
#ifndef BURNINGSYSTEM_HPP
#define BURNINGSYSTEM_HPP

#include "components.hpp"
#include "ecsSystem.hpp"
#include "ecsWorld.hpp"

///////////////////////////////////////////////////////////////////////////
/// Use the shared mini namespace
using namespace mini;

/////////////////////////////////////////////////////////////////////////
/// \class  BurningSystem
/// \brief  System used to lower the health of burning entities.
class BurningSystem final : public ecsSystem {
    public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Construct a cleanup system.
    explicit BurningSystem();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Tick this system by deltaTime.
    /// \param	deltaTime	    the amount of time passed since last update.
    /// \param	components	    the components to update.
    void updateComponents(
        const double&,
        const std::vector<std::vector<ecsBaseComponent*>>& entityComponents)
        final;
};

#endif // BURNINGSYSTEM_HPP