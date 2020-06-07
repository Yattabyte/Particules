#include "CollisionManifoldSystem.hpp"

//////////////////////////////////////////////////////////////////////
/// Custom Constructor
//////////////////////////////////////////////////////////////////////

CollisionManifoldSystem::CollisionManifoldSystem(
    ecsWorld& gameWorld,
    std::shared_ptr<ParticleComponent* [513][513]>& particleArray)
    : m_gameWorld(gameWorld), m_particleArray(particleArray) {
    addComponentType(ParticleComponent::Runtime_ID, RequirementsFlag::REQUIRED);
}

//////////////////////////////////////////////////////////////////////
/// updateComponents
//////////////////////////////////////////////////////////////////////

void CollisionManifoldSystem::updateComponents(
    const double& /*deltaTime*/,
    const std::vector<std::vector<ecsBaseComponent*>>& entityComponents) {
    for (const auto& components : entityComponents) {
        auto& particleComponent =
            *static_cast<ParticleComponent*>(components.front());
        const auto& entity1Handle = particleComponent.m_entityHandle;
        const auto entity1Pointer = m_gameWorld.getEntity(entity1Handle);
        const int x = static_cast<int>(particleComponent.m_pos.x());
        const int y = static_cast<int>(particleComponent.m_pos.y());
        std::vector<std::pair<ParticleComponent*, vec2>> collidingObjects;
        collidingObjects.reserve(8);

        // Left Side
        if (x > 0 && y > 0 && m_particleArray[y - 1][x - 1] != nullptr)
            collidingObjects.emplace_back(
                m_particleArray[y - 1][x - 1], vec2(-1, -1));
        if (x > 0 && m_particleArray[y][x - 1] != nullptr)
            collidingObjects.emplace_back(
                m_particleArray[y][x - 1], vec2(-1, 0));
        if (x > 0 && y < 511 && m_particleArray[y + 1][x - 1] != nullptr)
            collidingObjects.emplace_back(
                m_particleArray[y + 1][x - 1], vec2(-1, 1));
        // Middle
        if (y > 0 && m_particleArray[y - 1][x] != nullptr)
            collidingObjects.emplace_back(
                m_particleArray[y - 1][x], vec2(0, -1));
        if (y < 511 && m_particleArray[y + 1][x] != nullptr)
            collidingObjects.emplace_back(
                m_particleArray[y + 1][x], vec2(0, 1));
        // Right Side
        if (x < 511 && y > 0 && m_particleArray[y - 1][x + 1] != nullptr)
            collidingObjects.emplace_back(
                m_particleArray[y - 1][x + 1], vec2(1, -1));
        if (x < 511 && m_particleArray[y][x + 1] != nullptr)
            collidingObjects.emplace_back(
                m_particleArray[y][x + 1], vec2(1, 0));
        if (x < 511 && y < 511 && m_particleArray[y + 1][x + 1] != nullptr)
            collidingObjects.emplace_back(
                m_particleArray[y + 1][x + 1], vec2(1, 1));

        for (auto& [entity2, normal] : collidingObjects) {
            const auto& entityHandle2 = entity2->m_entityHandle;

            // If we hit, store the collision properties in a manifold
            m_gameWorld.makeComponent<CollisionManifoldComponent>(
                entity1Handle);
            auto manifoldComponent = static_cast<CollisionManifoldComponent*>(
                m_gameWorld.getComponent<CollisionManifoldComponent>(
                    *entity1Pointer));
            manifoldComponent->collisions.emplace_back(
                CollisionManifoldComponent::CollisionManifold{
                    m_gameWorld.getEntity(entityHandle2), normal });
        }
    }
}