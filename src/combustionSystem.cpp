#include "combustionSystem.hpp"

//////////////////////////////////////////////////////////////////////
/// Custom Constructor
//////////////////////////////////////////////////////////////////////

CombustionSystem::CombustionSystem(
    ecsWorld& gameWorld, const QuadTree<ParticleComponent*>& quadTree)
    : m_gameWorld(gameWorld), m_quadTree(quadTree) {
    addComponentType(ParticleComponent::Runtime_ID, RequirementsFlag::REQUIRED);
    addComponentType(
        ExplosiveComponent::Runtime_ID, RequirementsFlag::REQUIRED);
    addComponentType(OnFireComponent::Runtime_ID, RequirementsFlag::REQUIRED);
}

//////////////////////////////////////////////////////////////////////
/// updateComponents
//////////////////////////////////////////////////////////////////////

void CombustionSystem::updateComponents(
    const double& deltaTime,
    const std::vector<std::vector<ecsBaseComponent*>>& entityComponents) {
    std::vector<EntityHandle> entitiesToBecomeInnert;
    for (const auto& components : entityComponents) {
        auto& particleComponent =
            *static_cast<ParticleComponent*>(components[0]);
        auto& explosiveComponent =
            *static_cast<ExplosiveComponent*>(components[1]);
        const auto dt = static_cast<float>(deltaTime);

        // Only detonate entities whose fuse ran out
        if (explosiveComponent.fuseTime > 0.0001F) {
            explosiveComponent.fuseTime -= dt;
            continue;
        }

        const auto& position1 = particleComponent.m_pos;
        constexpr float forceRadius = 10.0F;
        constexpr float burningRadius = 5.0F;

        // Search the quad tree for possible targets
        for (auto possibleTarget : m_quadTree.search(position1, forceRadius)) {
            auto possibleTargetPointer =
                m_gameWorld.getEntity(possibleTarget->m_entityHandle);
            const auto& position2 = possibleTarget->m_pos;
            const auto& scale2 = possibleTarget->m_dimensions;

            // Avoid colliding against self
            if (&particleComponent == possibleTarget)
                continue;

            // Ignore anything out of the force radius
            vec2 deltaPos = position1 - position2;
            vec2 clamped = deltaPos;
            clamped.x() = std::clamp(clamped.x(), -scale2.x(), scale2.x());
            clamped.y() = std::clamp(clamped.y(), -scale2.y(), scale2.y());
            const vec2 closest = position2 + clamped;
            deltaPos = closest - position1;
            const float deltaLength = deltaPos.length();
            if (deltaLength > forceRadius)
                continue;

            // Apply force-damage to targets within radius
            // possibleTarget->m_health -=
            //     deltaLength * ((1.0F / burningRadius) * (1.0F /
            //     burningRadius));

            // Apply force to targets within radius
            if (auto physics2 = m_gameWorld.getComponent<PhysicsComponent>(
                    possibleTarget->m_entityHandle)) {
                // Only apply forces to particles with mass
                if (physics2->mass < 0.0001F)
                    continue;

                constexpr float damping = 0.9999F;
                const auto weightForce = deltaPos * 30.0F * physics2->mass;
                const auto dampingForce = physics2->m_velocity * -damping;

                physics2->m_velocity += (dampingForce + weightForce) * dt;
            }

            // Set targets within radius on fire
            if (deltaLength <= burningRadius &&
                m_gameWorld.getComponent<FlammableComponent>(
                    *possibleTargetPointer)) {
                m_gameWorld.makeComponent<OnFireComponent>(
                    *possibleTargetPointer);
            }
        }
        particleComponent.m_color = COLOR_SLUDGE;
        entitiesToBecomeInnert.emplace_back(particleComponent.m_entityHandle);
    }

    // Remove all identified entities
    for (const auto& handle : entitiesToBecomeInnert)
        m_gameWorld.removeComponent<ExplosiveComponent>(handle);
}