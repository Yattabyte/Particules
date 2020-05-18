#include "entityCleanupSystem.hpp"
#include "collision.hpp"

//////////////////////////////////////////////////////////////////////
/// Custom Constructor
//////////////////////////////////////////////////////////////////////

EntityCleanupSystem::EntityCleanupSystem(ecsWorld& gameWorld)
    : m_gameWorld(gameWorld) {
    addComponentType(ParticleComponent::Runtime_ID, RequirementsFlag::REQUIRED);
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

        // Find entities that fall outside the screen's bounds
        if (!areColliding_BoxVsBox(position, extents, vec2(0), vec2(250)))
            entitiesToDelete.emplace_back(particleComponent.m_entityHandle);
    }

    // Remove all out-of-bounds entities
    for (const auto& handle : entitiesToDelete)
        m_gameWorld.removeEntity(handle);
}