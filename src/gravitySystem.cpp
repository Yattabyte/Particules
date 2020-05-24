#include "gravitySystem.hpp"

//////////////////////////////////////////////////////////////////////
/// Custom Constructor
//////////////////////////////////////////////////////////////////////

GravitySystem::GravitySystem() {
    addComponentType(ParticleComponent::Runtime_ID, RequirementsFlag::REQUIRED);
    addComponentType(PhysicsComponent::Runtime_ID, RequirementsFlag::REQUIRED);
}

//////////////////////////////////////////////////////////////////////
/// updateComponents
//////////////////////////////////////////////////////////////////////

void GravitySystem::updateComponents(
    const double& deltaTime,
    const std::vector<std::vector<ecsBaseComponent*>>& entityComponents) {
    for (const auto& components : entityComponents) {
        auto& particleComponent =
            *static_cast<ParticleComponent*>(components[0]);
        auto& physicsComponent = *static_cast<PhysicsComponent*>(components[1]);

        // Only apply forces to particles with mass
        if (physicsComponent.mass < 0.0001F)
            continue;

        constexpr float damping = 0.9999F;
        const auto weightForce = vec2(0, physicsComponent.mass * -9.81F);
        const auto dampingForce = physicsComponent.m_velocity * -damping;

        const auto dt = static_cast<float>(deltaTime);
        particleComponent.m_pos += physicsComponent.m_velocity * dt;
        physicsComponent.m_velocity += (dampingForce + weightForce) * dt;
    }
}