#include "entityCleanupSystem.hpp"

EntityCleanupSystem::EntityCleanupSystem(ecsWorld& gameWorld)
    : m_gameWorld(gameWorld) {
    addComponentType(
        ParticleComponent::Runtime_ID, RequirementsFlag::FLAG_REQUIRED);
    addComponentType(
        BoundingBoxComponent::Runtime_ID, RequirementsFlag::FLAG_REQUIRED);
}

void EntityCleanupSystem::updateComponents(
    const double&,
    const std::vector<std::vector<ecsBaseComponent*>>& entityComponents) {
    std::vector<EntityHandle> entitiesToDelete;
    for (auto& components : entityComponents) {
        const auto& particleComponent =
            *static_cast<ParticleComponent*>(components[0]);
        const auto& boundingComponent =
            *static_cast<BoundingBoxComponent*>(components[1]);
        const auto& position = particleComponent.particle.m_pos;
        const auto& extents = boundingComponent.extents;

        if (position.x() - extents.x() > 250.0F ||
            position.x() + extents.x() < -250.0F ||
            position.y() - extents.y() > 250.0F ||
            position.y() + extents.y() < -250.0F)
            entitiesToDelete.emplace_back(particleComponent.m_entityHandle);
    }

    for (const auto& handle : entitiesToDelete)
        m_gameWorld.removeEntity(handle);
}