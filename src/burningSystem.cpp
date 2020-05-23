#include "burningSystem.hpp"

//////////////////////////////////////////////////////////////////////
/// Custom Constructor
//////////////////////////////////////////////////////////////////////

BurningSystem::BurningSystem() {
    addComponentType(ParticleComponent::Runtime_ID, RequirementsFlag::REQUIRED);
    addComponentType(OnFireComponent::Runtime_ID, RequirementsFlag::REQUIRED);
}

//////////////////////////////////////////////////////////////////////
/// updateComponents
//////////////////////////////////////////////////////////////////////

void BurningSystem::updateComponents(
    const double& deltaTime,
    const std::vector<std::vector<ecsBaseComponent*>>& entityComponents) {
    for (const auto& components : entityComponents) {
        auto& particleComponent =
            *static_cast<ParticleComponent*>(components[0]);
        particleComponent.m_health -= static_cast<float>(deltaTime);
    }
}