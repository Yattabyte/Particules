#include "movementDetectorSystem.hpp"

//////////////////////////////////////////////////////////////////////
/// Custom Constructor
//////////////////////////////////////////////////////////////////////

MovementDetectorSystem::MovementDetectorSystem(ecsWorld& gameWorld)
    : m_gameWorld(gameWorld) {
    addComponentType(PhysicsComponent::Runtime_ID, RequirementsFlag::REQUIRED);
}

//////////////////////////////////////////////////////////////////////
/// updateComponents
//////////////////////////////////////////////////////////////////////

void MovementDetectorSystem::updateComponents(
    const double& /*deltaTime*/,
    const std::vector<std::vector<ecsBaseComponent*>>& entityComponents) {
    for (const auto& componentSet : entityComponents) {
        const auto& entityHandle = componentSet.front()->m_entityHandle;
        auto& physics = *static_cast<PhysicsComponent*>(componentSet.front());

        if (physics.m_velocity.length() >= 0.0001F) {
            static const MovingComponent movingComponent;
            m_gameWorld.makeComponent(entityHandle, &movingComponent);
        } else {
            physics.m_velocity = vec2(0.0F);
            m_gameWorld.removeEntityComponent(
                entityHandle, MovingComponent::Runtime_ID);
        }
    }
}