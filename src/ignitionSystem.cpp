#include "ignitionSystem.hpp"

//////////////////////////////////////////////////////////////////////
/// Custom Constructor
//////////////////////////////////////////////////////////////////////

IgnitionSystem::IgnitionSystem(ecsWorld& gameWorld) : m_gameWorld(gameWorld) {
    addComponentType(
        CollisionManifoldComponent::Runtime_ID, RequirementsFlag::REQUIRED);
    addComponentType(OnFireComponent::Runtime_ID, RequirementsFlag::REQUIRED);
}

//////////////////////////////////////////////////////////////////////
/// updateComponents
//////////////////////////////////////////////////////////////////////

void IgnitionSystem::updateComponents(
    const double& /*deltaTime*/,
    const std::vector<std::vector<ecsBaseComponent*>>& entityComponents) {
    for (const auto& components : entityComponents) {
        const auto collisionManifold =
            static_cast<CollisionManifoldComponent*>(components.front());

        for (const auto& manifold : collisionManifold->collisions) {
            if (m_gameWorld.getComponent<FlammableComponent>(
                    *manifold.otherEntity)) {
                m_gameWorld.makeComponent<OnFireComponent>(
                    *manifold.otherEntity);
                static_cast<ParticleComponent*>(
                    m_gameWorld.getComponent<ParticleComponent>(
                        *manifold.otherEntity))
                    ->m_color = COLOR_FIRE;
            }
        }
    }
}