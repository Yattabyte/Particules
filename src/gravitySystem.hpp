#pragma once
#ifndef GRAVITYSYSTEM_HPP
#define GRAVITYSYSTEM_HPP

#include "components.hpp"
#include "ecsSystem.hpp"

///////////////////////////////////////////////////////////////////////////
/// Use the shared mini namespace
using namespace mini;

/////////////////////////////////////////////////////////////////////////
/// \class  GravitySystem
/// \brief  System used to apply gravity to entity components.
class GravitySystem final : public ecsSystem {
    public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Construct a gravity system.
    GravitySystem();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Tick this system by deltaTime.
    /// \param	deltaTime	    the amount of time passed since last update.
    /// \param	components	    the components to update.
    void updateComponents(
        const double& deltaTime,
        const std::vector<std::vector<ecsBaseComponent*>>& entityComponents)
        final;
};

#endif // GRAVITYSYSTEM_HPP