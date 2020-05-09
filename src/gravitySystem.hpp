#pragma once
#ifndef GRAVITYSYSTEM_HPP
#define GRAVITYSYSTEM_HPP

#include "components.hpp"
#include "ecsSystem.hpp"

/////////////////////////////////////////////////////////////////////////
/// \class  GravitySystem
/// \brief  System used to apply gravity to entity components.
class GravitySystem final : public mini::ecsSystem {
    public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Construct a gravity system.
    GravitySystem() {
        addComponentType(
            ParticleComponent::Runtime_ID, RequirementsFlag::FLAG_REQUIRED);
        addComponentType(
            MassComponent::Runtime_ID, RequirementsFlag::FLAG_REQUIRED);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Tick this system by deltaTime.
    /// \param	deltaTime	    the amount of time passed since last update.
    /// \param	components	    the components to update.
    void updateComponents(
        const double& deltaTime,
        const std::vector<std::vector<mini::ecsBaseComponent*>>&
            entityComponents) final {
        for (auto& components : entityComponents) {
            auto& particleComponent =
                *static_cast<ParticleComponent*>(components[0]);
            auto& massComponent = *static_cast<MassComponent*>(components[1]);

            const float force = massComponent.mass * -9.81F;
            particleComponent.particle.m_velocity.y() +=
                force * static_cast<float>(deltaTime);

            particleComponent.particle.m_pos +=
                particleComponent.particle.m_velocity *
                vec2(static_cast<float>(deltaTime));
        }
    }
};

#endif // GRAVITYSYSTEM_HPP