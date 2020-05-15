#include "movementDetectorSystem.hpp"

MovementDetectorSystem::MovementDetectorSystem(ecsWorld& gameWorld)
    : m_gameWorld(gameWorld) {
    addComponentType(
        ParticleComponent::Runtime_ID, RequirementsFlag::FLAG_REQUIRED);
}

void MovementDetectorSystem::updateComponents(
    const double&,
    const std::vector<std::vector<ecsBaseComponent*>>& entityComponents) {
    for (auto& componentSet : entityComponents) {
        auto& entityHandle = componentSet[0]->m_entityHandle;
        auto& particle =
            static_cast<ParticleComponent*>(componentSet[0])->particle;

        const auto velocityLength = std::sqrt(
            (particle.m_velocity.x() * particle.m_velocity.x()) +
            (particle.m_velocity.y() * particle.m_velocity.y()));
        if (std::abs(velocityLength) > 0.0001F) {
            static const MovingComponent movingComponent;
            m_gameWorld.makeComponent(entityHandle, &movingComponent);
        } else {
            particle.m_velocity = vec2(0.0F);
            m_gameWorld.removeEntityComponent(
                entityHandle, MovingComponent::Runtime_ID);
        }
    }
}