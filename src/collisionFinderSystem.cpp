#include "collisionFinderSystem.hpp"
#include "quadTree.hpp"

constexpr float tolerance = 0.0001F;

//////////////////////////////////////////////////////////////////////
/// resolveCollisions
//////////////////////////////////////////////////////////////////////

void CollisionFinderSystem::resolveCollisions(
    const double& /*deltaTime*/, ecsWorld& world) {
    // Retrieve a list of all physics objects
    // then insert them into a quad-tree
    const auto physicsEntities =
        world.getComponents<ParticleComponent*, BoundingBoxComponent*>(
            { { ParticleComponent::Runtime_ID,
                ecsSystem::RequirementsFlag::FLAG_REQUIRED },
              { BoundingBoxComponent::Runtime_ID,
                ecsSystem::RequirementsFlag::FLAG_REQUIRED } });
    QuadTree<std::tuple<ParticleComponent*, BoundingBoxComponent*>> tree(
        vec2(0), vec2(250.0F), 0);
    for (const auto& entity2 : physicsEntities) {
        const auto particleComponent = std::get<0>(entity2);
        const auto boundingComponent = std::get<1>(entity2);
        tree.insert(
            { particleComponent, boundingComponent },
            particleComponent->particle.m_pos, boundingComponent->extents);
    }

    // Next, for only the moving physics objects
    // check which ones are colliding and generate
    // collision manifolds for each of them
    const auto movingEntities = world.getComponents<
        ParticleComponent*, BoundingBoxComponent*, MovingComponent*>(
        { { ParticleComponent::Runtime_ID,
            ecsSystem::RequirementsFlag::FLAG_REQUIRED },
          { BoundingBoxComponent::Runtime_ID,
            ecsSystem::RequirementsFlag::FLAG_REQUIRED },
          { MovingComponent::Runtime_ID,
            ecsSystem::RequirementsFlag::FLAG_REQUIRED } });
    for (auto& entity1 : movingEntities) {
        const auto& entityHandle1 = std::get<0>(entity1)->m_entityHandle;
        auto& particle1 = std::get<0>(entity1)->particle;
        auto& extents1 = std::get<1>(entity1)->extents;
        const auto collidingObjects = tree.search(particle1.m_pos, extents1);

        for (auto& entity2 : collidingObjects) {
            const auto& entityHandle2 = std::get<0>(entity2)->m_entityHandle;
            auto& particle2 = std::get<0>(entity2)->particle;
            auto& extents2 = std::get<1>(entity2)->extents;

            // Avoid colliding against self
            if (&particle1 == &particle2)
                continue;

            const auto AABBvsAABB =
                [&]() noexcept -> std::tuple<bool, vec2, float> {
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
                                return { true, vec2(-1, 0), x_overlap };
                            else
                                return { true, vec2(1, 0), x_overlap };
                        } else {
                            // Point toward B knowing that n points from A
                            // to B
                            if (n.y() < tolerance)
                                return { true, vec2(0, -1), y_overlap };
                            else
                                return { true, vec2(0, 1), y_overlap };
                        }
                    }
                }
                return { false, vec2(0), 0.0F };
            };

            // If we hit, store the collision properties in a manifold
            const auto& [hit, normal, penetrationDepth] = AABBvsAABB();
            if (hit) {
                static const CollisionManifoldComponent tmpComponent;
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