#pragma once
#ifndef COLLISIONSYSTEM_HPP
#define COLLISIONSYSTEM_HPP

#include "collision.hpp"
#include "components.hpp"
#include "ecsSystem.hpp"
#include "ecsWorld.hpp"

/***/
class CollisionSystem final : public ecsBaseSystem {
    public:
    /***/
    CollisionSystem(ecsWorld& gameWorld) : m_gameWorld(gameWorld) {
        addComponentType(
            ParticleComponent::Runtime_ID, RequirementsFlag::FLAG_REQUIRED);
        addComponentType(
            BoundingSphereComponent::Runtime_ID,
            RequirementsFlag::FLAG_REQUIRED);
        addComponentType(
            MoveableComponent::Runtime_ID, RequirementsFlag::FLAG_REQUIRED);
    }
    // Public Interface Implementation
    void updateComponents(
        const double& deltaTime,
        const std::vector<std::vector<ecsBaseComponent*>>& entityComponents)
        final {
        for (auto& components : entityComponents) {
            auto& particle =
                static_cast<ParticleComponent*>(components[0])->particle;
            auto& radius =
                static_cast<BoundingSphereComponent*>(components[1])->radius;

            const std::vector<
                std::pair<ComponentID, ecsBaseSystem::RequirementsFlag>>
                nestedRequirements = { std::make_pair(
                                           ParticleComponent::Runtime_ID,
                                           RequirementsFlag::FLAG_REQUIRED),
                                       std::make_pair(
                                           BoundingBoxComponent::Runtime_ID,
                                           RequirementsFlag::FLAG_REQUIRED) };
            const auto nestedFunction =
                [&](const double&,
                    const std::vector<std::vector<ecsBaseComponent*>>& ec) {
                    for (auto& c : ec) {
                        auto& otherParticle =
                            static_cast<ParticleComponent*>(c[0])->particle;
                        auto& extents =
                            static_cast<BoundingBoxComponent*>(c[1])->extents;

                        if (areColliding_SphereVsBox(
                                particle.m_pos, radius, otherParticle.m_pos,
                                vec2(extents))) {
                            // Acquire specific collision information
                            const auto& [hit, intPoint, normal] =
                                rayBBoxIntersection(
                                    particle.m_pos,
                                    particle.m_velocity.normalize(),
                                    otherParticle.m_pos, vec2(extents), true);

                            // Retract movement to intersection point
                            // (offset by radius of particle)
                            particle.m_pos = intPoint + vec2(1.0F) * normal;

                            // Reflect the velocity off of the surface
                            particle.m_velocity =
                                (particle.m_velocity -
                                 vec2(2.0F) *
                                     vec2(particle.m_velocity.dot(normal)) *
                                     normal) *
                                vec2(0.5F);
                        }
                    }
                };

            m_gameWorld.updateSystem(
                deltaTime, nestedRequirements, nestedFunction);
        }
    }

    private:
    ecsWorld& m_gameWorld;
};

#endif // COLLISIONSYSTEM_HPP