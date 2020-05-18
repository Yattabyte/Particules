#include "collisionFinderSystem.hpp"

constexpr float tolerance = 0.0001F;

//////////////////////////////////////////////////////////////////////
/// findCollisions
//////////////////////////////////////////////////////////////////////

CollisionFinderSystem::CollisionFinderSystem()
    : m_quadTree(vec2(0), vec2(250.0F), 0) {}

void CollisionFinderSystem::findCollisions(
    const double& /*deltaTime*/, ecsWorld& world) {
    // Retrieve a list of all physics objects
    // then insert them into a quad-tree
    m_quadTree.clear();
    const auto physicsEntities = world.getComponents<ParticleComponent*>(
        { { ParticleComponent::Runtime_ID,
            ecsSystem::RequirementsFlag::REQUIRED } });
    for (const auto& entity2 : physicsEntities) {
        const auto& particleComponent = std::get<0>(entity2);
        m_quadTree.insert(
            particleComponent, particleComponent->m_pos,
            particleComponent->m_dimensions);
    }

    // Next, for only the moving physics objects
    // check which ones are colliding and generate
    // collision manifolds for each of them
    const auto movingEntities =
        world.getComponents<ParticleComponent*, MovingComponent*>(
            { { ParticleComponent::Runtime_ID,
                ecsSystem::RequirementsFlag::REQUIRED },
              { MovingComponent::Runtime_ID,
                ecsSystem::RequirementsFlag::REQUIRED } });
    for (const auto& entity1 : movingEntities) {
        const auto& entityHandle1 = std::get<0>(entity1)->m_entityHandle;
        const auto& particle1 = *std::get<0>(entity1);
        const auto collidingObjects =
            m_quadTree.search(particle1.m_pos, particle1.m_dimensions);

        for (auto& entity2 : collidingObjects) {
            const auto& entityHandle2 = entity2->m_entityHandle;
            const auto& particle2 = *entity2;

            // Avoid colliding against self
            if (&particle1 == &particle2)
                continue;

            const auto AABBvsAABB =
                [&]() noexcept -> std::tuple<bool, vec2, float> {
                // Vector from A to B
                const vec2 n = particle2.m_pos - particle1.m_pos;

                // Calculate overlap on x axis
                const float x_overlap = particle1.m_dimensions.x() +
                                        particle2.m_dimensions.x() -
                                        std::abs(n.x());

                // SAT test on x axis
                if (x_overlap > tolerance) {
                    // Calculate overlap on y axis
                    const float y_overlap = particle1.m_dimensions.y() +
                                            particle2.m_dimensions.y() -
                                            std::abs(n.y());

                    // SAT test on y axis
                    if (y_overlap > tolerance) {
                        // Find out which axis is axis of least penetration
                        if (x_overlap < y_overlap) {
                            // Point towards B knowing that n points from A
                            // to B
                            if (n.x() < tolerance)
                                return { true, vec2(-1, 0), x_overlap };
                            return { true, vec2(1, 0), x_overlap };
                        }
                        // Point toward B knowing that n points from A
                        // to B
                        if (n.y() < tolerance)
                            return { true, vec2(0, -1), y_overlap };
                        return { true, vec2(0, 1), y_overlap };
                    }
                }
                return { false, vec2(0), 0.0F };
            };

            // If we hit, store the collision properties in a manifold
            const auto& [hit, normal, penetrationDepth] = AABBvsAABB();
            if (hit && std::abs(penetrationDepth) >= 0.0001F) {
                static const CollisionManifoldComponent tmpComponent;
                [[maybe_unused]] const auto componentHandle =
                    world.makeComponent(entityHandle1, &tmpComponent);
                auto manifoldComponent =
                    static_cast<CollisionManifoldComponent*>(world.getComponent(
                        entityHandle1, CollisionManifoldComponent::Runtime_ID));
                manifoldComponent->collisions.emplace_back(
                    CollisionManifoldComponent::CollisionManifold{
                        entityHandle2, normal, penetrationDepth });
            }
        }
    }
}