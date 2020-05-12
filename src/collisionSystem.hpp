#pragma once
#ifndef COLLISIONSYSTEM_HPP
#define COLLISIONSYSTEM_HPP

#include "collision.hpp"
#include "components.hpp"
#include "ecsSystem.hpp"
#include "ecsWorld.hpp"

constexpr float tolerance = 0.00001F;

///////////////////////////////////////////////////////////////////////////
/// Use the shared mini namespace
using namespace mini;

///////////////////////////////////////////////////////////////////////////
/// \class  CollisionSystem
/// \brief  System used to check if components collide in an ECS.
class CollisionSystem final : public ecsSystem {
    public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Construct a collision system with an ecsWorld reference.
    /// \param  gameWorld   reference to the engine's game world.
    CollisionSystem(ecsWorld& gameWorld) : m_gameWorld(gameWorld) {
        addComponentType(
            ParticleComponent::Runtime_ID, RequirementsFlag::FLAG_REQUIRED);
        addComponentType(
            BoundingBoxComponent::Runtime_ID, RequirementsFlag::FLAG_REQUIRED);
        addComponentType(
            PhysicsComponent::Runtime_ID, RequirementsFlag::FLAG_REQUIRED);
        addComponentType(
            MoveableComponent::Runtime_ID, RequirementsFlag::FLAG_REQUIRED);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Tick this system by deltaTime.
    /// \param	deltaTime	    the amount of time passed since last update.
    /// \param	components	    the components to update.
    void updateComponents(
        const double& deltaTime,
        const std::vector<std::vector<ecsBaseComponent*>>& entityComponents)
        final {
        for (auto& components : entityComponents) {
            m_particle = static_cast<ParticleComponent*>(components[0]);
            m_bounds = static_cast<BoundingBoxComponent*>(components[1]);
            m_physics = static_cast<PhysicsComponent*>(components[2]);

            // Check collision of this particle versus the world
            m_gameWorld.updateSystem(
                deltaTime,
                { { ParticleComponent::Runtime_ID,
                    RequirementsFlag::FLAG_REQUIRED },
                  { BoundingBoxComponent::Runtime_ID,
                    RequirementsFlag::FLAG_REQUIRED },
                  { PhysicsComponent::Runtime_ID,
                    RequirementsFlag::FLAG_REQUIRED } },
                [&](const double& dt,
                    const std::vector<std::vector<ecsBaseComponent*>>& ec) {
                    particleVersusWorld(dt, ec);
                });
        }
    }

    void particleVersusWorld(
        const double&,
        const std::vector<std::vector<ecsBaseComponent*>>& entityComponents) {
        auto& particle = m_particle->particle;
        const auto& extents = m_bounds->extents;
        const auto& physics = *m_physics;
        for (auto& components : entityComponents) {
            auto& otherParticle =
                static_cast<ParticleComponent*>(components[0])->particle;
            const auto& otherExtents =
                static_cast<BoundingBoxComponent*>(components[1])->extents;
            const auto& otherPhysics =
                *static_cast<PhysicsComponent*>(components[2]);

            // Avoid colliding against self
            if (&otherParticle == &particle)
                continue;

            const auto AABBvsAABB = [&]() {
                // Vector from A to B
                const vec2 n = otherParticle.m_pos - particle.m_pos;

                const auto aBoxMax = particle.m_pos + extents;
                const auto aBoxMin = particle.m_pos - extents;
                const auto bBoxMax = otherParticle.m_pos + otherExtents;
                const auto bBoxMin = otherParticle.m_pos - otherExtents;

                // Calculate half extents along x axis for each object
                const float a_extentX = (aBoxMax.x() - aBoxMin.x()) / 2.0F;
                const float b_extentX = (bBoxMax.x() - bBoxMin.x()) / 2.0F;

                // Calculate overlap on x axis
                const float x_overlap = a_extentX + b_extentX - std::abs(n.x());

                // SAT test on x axis
                if (x_overlap > tolerance) {
                    // Calculate half extents along y axis for each object
                    const float a_extentY = (aBoxMax.y() - aBoxMin.y()) / 2.0F;
                    const float b_extentY = (bBoxMax.y() - bBoxMin.y()) / 2.0F;

                    // Calculate overlap on y axis
                    const float y_overlap =
                        a_extentY + b_extentY - std::abs(n.y());

                    // SAT test on y axis
                    if (y_overlap > tolerance) {
                        // Find out which axis is axis of least penetration
                        if (x_overlap < y_overlap) {
                            // Point towards B knowing that n points from A to B
                            if (n.x() < tolerance)
                                return std::make_tuple(
                                    true, vec2(-1, 0), x_overlap);
                            else
                                return std::make_tuple(
                                    true, vec2(1, 0), x_overlap);
                        } else {
                            // Point toward B knowing that n points from A to B
                            if (n.y() < tolerance)
                                return std::make_tuple(
                                    true, vec2(0, -1), y_overlap);
                            else
                                return std::make_tuple(
                                    true, vec2(0, 1), y_overlap);
                        }
                    }
                }
                return std::make_tuple(false, vec2(0, 0), 0.0F);
            };

            // Ensure we hit
            const auto& [hit, normal, penetrationDepth] = AABBvsAABB();
            if (!hit)
                continue;

            // Do not resolve if depth is beneath tolerance
            if (penetrationDepth < tolerance)
                continue;

            // Calculate relative velocity
            const vec2 relativeVelocity =
                otherParticle.m_velocity - particle.m_velocity;

            // Calculate relative velocity in terms of the normal direction
            const float velocityAlongNormal = relativeVelocity.dot(normal);

            // Do not resolve if velocities are separating
            if (-velocityAlongNormal < tolerance)
                continue;

            // Calculate restitution
            const float e =
                std::min(physics.restitution, otherPhysics.restitution);

            // Calculate impulse scalar
            const float j = -((1.0F + e) * velocityAlongNormal) /
                            (physics.mass + otherPhysics.mass);

            // Apply impulse
            const vec2 impulse = vec2(j) * normal;
            particle.m_velocity -= vec2(physics.mass) * impulse;
            otherParticle.m_velocity += vec2(otherPhysics.mass) * impulse;

            // Correct position
            constexpr float percent = 0.5F;
            constexpr float slop = 0.1F;
            vec2 correction =
                vec2(
                    std::max(penetrationDepth - slop, 0.0F) /
                    (physics.mass + otherPhysics.mass) * percent) *
                normal;
            particle.m_pos -= vec2(physics.mass) * correction;
            otherParticle.m_pos += vec2(otherPhysics.mass) * correction;
        }
    };

    private:
    ecsWorld& m_gameWorld; ///< Reference to the engine's game world.
    ParticleComponent* m_particle = nullptr;  ///< Particle component.
    BoundingBoxComponent* m_bounds = nullptr; ///< Bounding component.
    PhysicsComponent* m_physics = nullptr;    ///< Physics component.
};

#endif // COLLISIONSYSTEM_HPP