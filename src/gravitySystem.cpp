#include "gravitySystem.hpp"

//////////////////////////////////////////////////////////////////////
/// Custom Constructor
//////////////////////////////////////////////////////////////////////

GravitySystem::GravitySystem() {
    addComponentType(
        ParticleComponent::Runtime_ID, RequirementsFlag::FLAG_REQUIRED);
    addComponentType(
        PhysicsComponent::Runtime_ID, RequirementsFlag::FLAG_REQUIRED);
}

//////////////////////////////////////////////////////////////////////
/// updateComponents
//////////////////////////////////////////////////////////////////////

void GravitySystem::updateComponents(
    const double& deltaTime,
    const std::vector<std::vector<ecsBaseComponent*>>& entityComponents) {
    for (auto& components : entityComponents) {
        auto& particleComponent =
            *static_cast<ParticleComponent*>(components[0]);
        const auto& massComponent =
            *static_cast<PhysicsComponent*>(components[1]);

        constexpr float damping = 0.9999F;
        const auto weightForce = vec2(0, massComponent.mass * -9.81F);
        const auto dampingForce =
            particleComponent.particle.m_velocity * -damping;

        const auto dt = static_cast<float>(deltaTime);
        particleComponent.particle.m_velocity +=
            (dampingForce + weightForce) * dt;
        particleComponent.particle.m_pos +=
            particleComponent.particle.m_velocity * dt;
    }
}