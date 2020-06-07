#include "burningSystem.hpp"

//////////////////////////////////////////////////////////////////////
/// Custom Constructor
//////////////////////////////////////////////////////////////////////

BurningSystem::BurningSystem(ecsWorld& gameWorld) : m_gameWorld(gameWorld) {
    addComponentType(ParticleComponent::Runtime_ID, RequirementsFlag::REQUIRED);
    addComponentType(
        FlammableComponent::Runtime_ID, RequirementsFlag::REQUIRED);
    addComponentType(OnFireComponent::Runtime_ID, RequirementsFlag::REQUIRED);
}

//////////////////////////////////////////////////////////////////////
/// updateComponents
//////////////////////////////////////////////////////////////////////

void BurningSystem::updateComponents(
    const double& deltaTime,
    const std::vector<std::vector<ecsBaseComponent*>>& entityComponents) {
    const auto dt = static_cast<float>(deltaTime);
    std::vector<EntityHandle> entitiesToExtinguish;
    for (const auto& components : entityComponents) {
        auto& particleComponent =
            *static_cast<ParticleComponent*>(components[0]);
        auto& flammableComponent =
            *static_cast<FlammableComponent*>(components[1]);

        // Check if this entity has burned up
        if (flammableComponent.wickTime <= 0.0001F) {
            particleComponent.m_color = COLOR_SLUDGE;
            entitiesToExtinguish.emplace_back(
                flammableComponent.m_entityHandle);
            continue;
        }

        particleComponent.m_health -= dt;
        flammableComponent.wickTime -= dt;
    }

    // Extinguish all entities who's wicks burned out
    for (const auto& handle : entitiesToExtinguish) {
        m_gameWorld.removeComponent<FlammableComponent>(handle);
        m_gameWorld.removeComponent<OnFireComponent>(handle);
    }
}