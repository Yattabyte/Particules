#include "findSleepingParticles.hpp"

FindSleepingParticlesSystem::FindSleepingParticlesSystem() {
    addComponentType(
        ParticleComponent::Runtime_ID, RequirementsFlag::FLAG_REQUIRED);
    addComponentType(
        MovingComponent::Runtime_ID, RequirementsFlag::FLAG_OPTIONAL);
}

void FindSleepingParticlesSystem::updateComponents(
    const double&,
    const std::vector<std::vector<ecsBaseComponent*>>& entityComponents) {
    size_t asleepCount(0);
    for (const auto& componentSet : entityComponents)
        if (!componentSet[1])
            ++asleepCount;

    if (asleepCount != 3)
        [[maybe_unused]] bool qwe = false;
}