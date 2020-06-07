#include "collisionCleanupSystem.hpp"

//////////////////////////////////////////////////////////////////////
/// Custom Constructor
//////////////////////////////////////////////////////////////////////

CollisionCleanupSystem::CollisionCleanupSystem(ecsWorld& gameWorld)
    : m_gameWorld(gameWorld) {
    addComponentType(
        CollisionManifoldComponent::Runtime_ID, RequirementsFlag::REQUIRED);
}

//////////////////////////////////////////////////////////////////////
/// updateComponents
//////////////////////////////////////////////////////////////////////

void CollisionCleanupSystem::updateComponents(
    const double& /*deltaTime*/,
    const std::vector<std::vector<ecsBaseComponent*>>& entityComponents) {
    std::vector<EntityHandle> entitiesToClean;
    entitiesToClean.reserve(entityComponents.size());
    for (const auto& components : entityComponents) {
        auto collisionComponent =
            static_cast<CollisionManifoldComponent*>(components.front());
        if (collisionComponent->collisions.empty())
            entitiesToClean.emplace_back(components.front()->m_entityHandle);
        else
            collisionComponent->collisions.clear();
    }

    for (const auto& handle : entitiesToClean)
        m_gameWorld.removeComponent<CollisionManifoldComponent>(handle);
}