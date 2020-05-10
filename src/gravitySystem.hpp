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
    GravitySystem() {
        addComponentType(
            ParticleComponent::Runtime_ID, RequirementsFlag::FLAG_REQUIRED);
        addComponentType(
            PhysicsComponent::Runtime_ID, RequirementsFlag::FLAG_REQUIRED);
        addComponentType(
            MoveableComponent::Runtime_ID, RequirementsFlag::FLAG_REQUIRED);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Tick this system by deltaTime.
    /// \param	deltaTime	    the amount of time passed since last update.
    /// \param	components	    the components to update.
    void updateComponents(
        const double& deltaTime,
        const std::vector<std::vector<ecsBaseComponent*>>& entityComponents)
        final {
        for (auto& components : entityComponents) {
            auto& particleComponent =
                *static_cast<ParticleComponent*>(components[0]);
            const auto& massComponent =
                *static_cast<PhysicsComponent*>(components[1]);

            const float force = massComponent.mass * -9.81F;
            particleComponent.particle.m_velocity.y() +=
                force * static_cast<float>(deltaTime);

            constexpr float damping = 0.9999F;
            particleComponent.particle.m_pos +=
                particleComponent.particle.m_velocity *
                vec2(static_cast<float>(deltaTime) * damping);
        }
    }
};

#endif // GRAVITYSYSTEM_HPP