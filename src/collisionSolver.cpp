#include "collisionSolver.hpp"

void CollisionSolver::resolveCollisions(const double&, ecsWorld& world) {
    // Retrieve a list of all move-able physics objects
    auto movingEntities = world.getComponents<
        ParticleComponent*, BoundingBoxComponent*, PhysicsComponent*,
        MoveableComponent*>({ { ParticleComponent::Runtime_ID,
                                ecsSystem::RequirementsFlag::FLAG_REQUIRED },
                              { BoundingBoxComponent::Runtime_ID,
                                ecsSystem::RequirementsFlag::FLAG_REQUIRED },
                              { PhysicsComponent::Runtime_ID,
                                ecsSystem::RequirementsFlag::FLAG_REQUIRED },
                              { MoveableComponent::Runtime_ID,
                                ecsSystem::RequirementsFlag::FLAG_REQUIRED } });

    // Retrieve a list of all interact-able physics objects
    auto physicsEntities = world.getComponents<
        ParticleComponent*, BoundingBoxComponent*, PhysicsComponent*>(
        { { ParticleComponent::Runtime_ID,
            ecsSystem::RequirementsFlag::FLAG_REQUIRED },
          { BoundingBoxComponent::Runtime_ID,
            ecsSystem::RequirementsFlag::FLAG_REQUIRED },
          { PhysicsComponent::Runtime_ID,
            ecsSystem::RequirementsFlag::FLAG_REQUIRED } });

    // Check if each move-able entity is colliding
    // against every other physics entity
    for (auto& entity1 : movingEntities) {
        auto& particle1 = std::get<0>(entity1)->particle;
        auto& extents1 = std::get<1>(entity1)->extents;
        auto& physics1 = *std::get<2>(entity1);

        for (auto& entity2 : physicsEntities) {
            auto& particle2 = std::get<0>(entity2)->particle;
            auto& extents2 = std::get<1>(entity2)->extents;
            auto& physics2 = *std::get<2>(entity2);

            // Avoid colliding against self
            if (&particle1 == &particle2)
                continue;

            const auto AABBvsAABB = [&]() {
                // Vector from A to B
                const vec2 n = particle2.m_pos - particle1.m_pos;

                const auto aBoxMax = particle1.m_pos + extents1;
                const auto aBoxMin = particle1.m_pos - extents1;
                const auto bBoxMax = particle2.m_pos + extents2;
                const auto bBoxMin = particle2.m_pos - extents2;

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
                            // Point towards B knowing that n points from A
                            // to B
                            if (n.x() < tolerance)
                                return std::make_tuple(
                                    true, vec2(-1, 0), x_overlap);
                            else
                                return std::make_tuple(
                                    true, vec2(1, 0), x_overlap);
                        } else {
                            // Point toward B knowing that n points from A
                            // to B
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
                particle2.m_velocity - particle1.m_velocity;

            // Calculate relative velocity in terms of the normal direction
            const float velocityAlongNormal = relativeVelocity.dot(normal);

            // Do not resolve if velocities are separating
            if (-velocityAlongNormal < tolerance)
                continue;

            // Calculate restitution
            const float e =
                std::min(physics1.restitution, physics2.restitution);

            // Calculate impulse scalar
            const float j = -((1.0F + e) * velocityAlongNormal) /
                            (physics1.mass + physics2.mass);

            // Apply impulse
            const vec2 impulse = vec2(j) * normal;
            particle1.m_velocity -= vec2(physics1.mass) * impulse;
            particle2.m_velocity += vec2(physics2.mass) * impulse;

            // Correct position
            constexpr float percent = 0.5F;
            constexpr float slop = 0.1F;
            vec2 correction = vec2(
                                  std::max(penetrationDepth - slop, 0.0F) /
                                  (physics1.mass + physics2.mass) * percent) *
                              normal;
            particle1.m_pos -= vec2(physics1.mass) * correction;
            particle2.m_pos += vec2(physics2.mass) * correction;
        }
    }
}