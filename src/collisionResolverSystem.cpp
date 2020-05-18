#include "collisionResolverSystem.hpp"

//////////////////////////////////////////////////////////////////////
/// resolveCollisions
//////////////////////////////////////////////////////////////////////

void CollisionResolverSystem::resolveCollisions(
    const double& /*deltaTime*/, ecsWorld& world) {
    // Retrieve a list of all move-able physics objects
    auto collidingEntities = world.getComponents<
        ParticleComponent*, PhysicsComponent*, CollisionManifoldComponent*>(
        { { ParticleComponent::Runtime_ID,
            ecsSystem::RequirementsFlag::REQUIRED },
          { PhysicsComponent::Runtime_ID,
            ecsSystem::RequirementsFlag::REQUIRED },
          { CollisionManifoldComponent::Runtime_ID,
            ecsSystem::RequirementsFlag::REQUIRED } });

    // Check if each move-able entity is colliding
    // against every other physics entity
    for (auto& entity1 : collidingEntities) {
        auto& particle1 = *std::get<0>(entity1);
        auto& physics1 = *std::get<1>(entity1);

        for (const auto& manifold : std::get<2>(entity1)->collisions) {
            const auto& [otherHandle, normal, depth] = manifold;
            auto& particle2 =
                *static_cast<ParticleComponent*>(world.getComponent(
                    manifold.otherEntity, ParticleComponent::Runtime_ID));
            auto& physics2 = *static_cast<PhysicsComponent*>(world.getComponent(
                manifold.otherEntity, PhysicsComponent::Runtime_ID));

            // Calculate relative velocity
            const vec2 relativeVelocity =
                physics2.m_velocity - physics1.m_velocity;

            // Calculate relative velocity in terms of the normal direction
            const float velocityAlongNormal = relativeVelocity.dot(normal);

            // Do not resolve if velocities are separating
            if (-velocityAlongNormal < 0.0001F)
                continue;

            // Calculate restitution
            const float e =
                std::min(physics1.restitution, physics2.restitution);

            // Calculate impulse scalar
            const float j = -((1.0F + e) * velocityAlongNormal) /
                            (physics1.mass + physics2.mass);

            // Apply impulse
            const vec2 impulse = normal * j;
            physics1.m_velocity -= impulse * physics1.mass;
            physics2.m_velocity += impulse * physics2.mass;

            // Correct position
            constexpr float percent = 0.8F;
            constexpr float slop = 0.01F;
            const vec2 correction = normal * std::max(depth - slop, 0.0F) /
                                    (physics1.mass + physics2.mass) * percent;
            particle1.m_pos -= correction * physics1.mass;
            particle2.m_pos += correction * physics2.mass;
        }
    }
}