#include "collisionCleanupSystem.hpp"

//////////////////////////////////////////////////////////////////////
/// Custom Constructor
//////////////////////////////////////////////////////////////////////

CollisionCleanupSystem::CollisionCleanupSystem(ecsWorld& gameWorld)
    : m_gameWorld(gameWorld) {
    addComponentType(
        CollisionManifoldComponent::Runtime_ID,
        RequirementsFlag::FLAG_REQUIRED);
}

//////////////////////////////////////////////////////////////////////
/// updateComponents
//////////////////////////////////////////////////////////////////////

void CollisionCleanupSystem::updateComponents(
    const double& /*deltaTime*/,
    const std::vector<std::vector<ecsBaseComponent*>>& entityComponents) {
    for (auto& components : entityComponents)
        static_cast<CollisionManifoldComponent*>(components.front())
            ->collisions.clear();
}