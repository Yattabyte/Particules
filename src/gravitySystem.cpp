#include "gravitySystem.hpp"

//////////////////////////////////////////////////////////////////////
/// Custom Constructor
//////////////////////////////////////////////////////////////////////

GravitySystem::GravitySystem() {
    addComponentType(ParticleComponent::Runtime_ID, RequirementsFlag::REQUIRED);
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

        // Only apply forces to particles with mass
        if (particleComponent.mass < 0.0001F)
            continue;

        constexpr float damping = 0.9999F;
        const auto weightForce = vec2(0, particleComponent.mass * -9.81F);
        const auto dampingForce = particleComponent.m_velocity * -damping;

        const auto dt = static_cast<float>(deltaTime);
        particleComponent.m_pos += particleComponent.m_velocity * dt;
        particleComponent.m_velocity += (dampingForce + weightForce) * dt;
    }
}