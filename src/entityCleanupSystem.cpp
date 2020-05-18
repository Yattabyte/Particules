#include "entityCleanupSystem.hpp"

//////////////////////////////////////////////////////////////////////
/// Custom Constructor
//////////////////////////////////////////////////////////////////////

EntityCleanupSystem::EntityCleanupSystem(ecsWorld& gameWorld)
    : m_gameWorld(gameWorld) {
    addComponentType(
        ParticleComponent::Runtime_ID, RequirementsFlag::FLAG_REQUIRED);
}

//////////////////////////////////////////////////////////////////////
/// updateComponents
//////////////////////////////////////////////////////////////////////

void EntityCleanupSystem::updateComponents(
    const double& /*deltaTime*/,
    const std::vector<std::vector<ecsBaseComponent*>>& entityComponents) {
    std::vector<EntityHandle> entitiesToDelete;
    for (const auto& components : entityComponents) {
        const auto& particleComponent =
            *static_cast<ParticleComponent*>(components[0]);
        const auto& position = particleComponent.m_pos;
        const auto& extents = particleComponent.m_dimensions;

        if (position.x() - extents.x() > 250.0F ||
            position.x() + extents.x() < -250.0F ||
            position.y() - extents.y() > 250.0F ||
            position.y() + extents.y() < -250.0F)
            entitiesToDelete.emplace_back(particleComponent.m_entityHandle);
    }

    for (const auto& handle : entitiesToDelete)
        m_gameWorld.removeEntity(handle);
}