#include "collisionFinderSystem.hpp"
#include <array>

constexpr float tolerance = 0.0001F;

//////////////////////////////////////////////////////////////////////
/// updateComponents
//////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
/// \brief  Construct a collision finder system.

CollisionFinderSystem::CollisionFinderSystem()
    : m_quadTree(vec2(0), vec2(250.0F), 0), m_particleArray(500) {
    std::fill(
        m_particleArray.begin(), m_particleArray.end(),
        std::vector<ParticleComponent*>(500, nullptr));
}

void CollisionFinderSystem::findCollisions(
    const double& deltaTime, ecsWorld& world) {
    // Retrieve a list of all physics objects
    // then insert them into a quad-tree
    m_quadTree.clear();
    const auto physicsEntities = world.getComponents<ParticleComponent*>(
        { { ParticleComponent::Runtime_ID,
            ecsSystem::RequirementsFlag::REQUIRED } });

    std::fill(
        m_particleArray.begin(), m_particleArray.end(),
        std::vector<ParticleComponent*>(500, nullptr));

    // Loop 1: Populate particle array
    for (const auto& entity2 : physicsEntities) {
        auto& particleComponent = *std::get<0>(entity2);

        const int x = static_cast<int>(particleComponent.m_pos.x());
        const int y = static_cast<int>(particleComponent.m_pos.y());
        if (x < 0 || x > 499 || y < 0 || y > 499)
            continue;

        if (m_particleArray[y][x] == nullptr) {
            m_particleArray[y][x] = &particleComponent;
        } else {
            // Two particles competing for same cell
            const bool thisCanMove = particleComponent.mass > tolerance;
            const bool thatCanMove = m_particleArray[y][x]->mass > tolerance;

            if (!thisCanMove && !thatCanMove) {
                [[maybe_unused]] bool thisShouldNeverHappen = false;
            }

            if (!thisCanMove && thatCanMove) {
                // This particle gets the cell, it moves away
                [[maybe_unused]] bool thisShouldNeverHappen = false;
            } else if (thisCanMove && !thatCanMove) {
                // That particle keeps the cell, we move away
                if (particleComponent.mass < tolerance) {
                    [[maybe_unused]] bool qwe = false;
                } else {
                    if (m_particleArray[y + 1][x] != nullptr) {
                        [[maybe_unused]] bool qwe = false;
                        /*m_particleArray[x].insert(
                            m_particleArray[x].begin() + y + 1,
                            &particleComponent);

                        int newY = y + 1;
                        for (auto otherParticleIt =
                                 m_particleArray[x].begin() + newY;
                             otherParticleIt != m_particleArray[x].end();
                             ++otherParticleIt) {
                            if (*otherParticleIt != nullptr)
                                (*otherParticleIt)->m_pos.y() =
                                    static_cast<float>(newY);
                            ++newY;
                        }
                    } else {
                        m_particleArray[x][y + 1] = &particleComponent;
                        particleComponent.m_pos.y() += 1.0F;*/
                    }
                }
            }
        }
    }

    // Apply Gravity and Velocity
    for (int y = 0; y < 500; ++y) {
        for (int x = 0; x < 500; ++x) {
            auto particle1 = m_particleArray[y][x];

            // Only act on actual particles
            if (particle1 == nullptr)
                continue;

            // Only apply forces to particles with mass
            if (particle1->mass < 0.0001F)
                continue;

            constexpr float damping = 0.5F;
            const auto weightForce = vec2(0, particle1->mass * -9.81F);
            const auto dampingForce = particle1->m_velocity * -damping;

            const auto dt = static_cast<float>(deltaTime);
            const vec2 startPos = particle1->m_pos;
            const vec2 endPos = particle1->m_pos + particle1->m_velocity * dt;
            const vec2 deltaPos = endPos - startPos;
            particle1->m_velocity += (dampingForce + weightForce) * dt;

            // Avoid dividing by zero
            if (deltaPos.length() < 0.0001F)
                continue;

            const vec2 dir = deltaPos.normalize();
            const int endPosX = static_cast<int>(endPos.x());
            const int endPosY = static_cast<int>(endPos.y());
            const int numCells = std::abs(endPosX - x) + std::abs(endPosY - y);
            const vec2 step = deltaPos / static_cast<float>(numCells);

            // Increment position and see if occluded
            int intX = 0;
            int intY = 0;
            for (int spot = 0; spot < numCells; ++spot) {
                vec2 testPos = particle1->m_pos + step;
                intX = static_cast<int>(testPos.x());
                intY = static_cast<int>(testPos.y());

                // Break if leaving the screen
                if (intX < 0 || intX > 499 || intY < 0 || intY > 499) {
                    // Delete
                    m_particleArray[y][x] = nullptr;
                    break;
                }

                else if (m_particleArray[intY][intX] != nullptr) {
                    // Moving downwards, check diagonally
                    if (dir.y() < 0.0F && dir.x() >= -0.5F && dir.x() <= 0.5F &&
                        x > 0 && x < 499) {
                        // Check if bottom left is free
                        if (m_particleArray[intY][intX - 1] == nullptr) {
                            testPos.x() -= 1;
                            intX -= 1;
                        }
                        // Check if bottom right is free
                        else if (m_particleArray[intY][intX + 1] == nullptr) {
                            testPos.x() += 1;
                            intX += 1;
                        } else
                            break;
                    } else
                        break;
                }

                particle1->m_pos = testPos;
                std::swap(m_particleArray[y][x], m_particleArray[intY][intX]);
            }
            /*
            // Handle Collision
            auto& particle2 = m_particleArray[newY][newX];
            const vec2 normal =
                vec2(static_cast<float>(newX - x), static_cast<float>(newY - y))
                    .normalize();

            // Calculate relative velocity
            const vec2 relativeVelocity =
                particle2->m_velocity - particle1->m_velocity;

            // Calculate relative velocity in terms of the normal
            // direction
            const float velocityAlongNormal = relativeVelocity.dot(normal);

            // Do not resolve if velocities are separating
            if (-velocityAlongNormal < 0.0001F)
                continue;

            // Calculate restitution
            const float e =
                std::min(particle1->restitution, particle2->restitution);

            // Calculate impulse scalar
            const float j = -((1.0F + e) * velocityAlongNormal) /
                            (particle1->mass + particle2->mass);

            // Apply impulse
            const vec2 impulse = normal * j;
            particle1->m_velocity -= impulse * particle1->mass;
            particle2->m_velocity += impulse * particle2->mass;

            // Correct position
            constexpr float percent = 0.8F;
            constexpr float slop = 0.01F;
            const vec2 correction = normal * std::max(0.5F - slop, 0.0F) /
                                    (particle1->mass + particle2->mass) *
                                    percent;
            particle1->m_pos -= correction * particle1->mass;
            particle2->m_pos += correction * particle2->mass*/
        }
    }

    /*// Next, for only the moving physics objects
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

        std::vector<std::pair<ParticleComponent*, vec2>> collidingObjects;
        collidingObjects.reserve(8);
        const int x = static_cast<int>(particle1.m_pos.x());
        const int y = static_cast<int>(particle1.m_pos.y());
        if (x < 0 || x > 499 || y < 0 || y > 499)
            continue;

        // Left Side
        if (x > 0 && y > 0 && m_particleArray[x - 1][y - 1] != nullptr)
            collidingObjects.emplace_back(
                m_particleArray[x - 1][y - 1], vec2(-1, -1));
        if (x > 0 && m_particleArray[x - 1][y] != nullptr)
            collidingObjects.emplace_back(
                m_particleArray[x - 1][y], vec2(-1, 0));
        if (x > 0 && y < 499 && m_particleArray[x - 1][y + 1] != nullptr)
            collidingObjects.emplace_back(
                m_particleArray[x - 1][y + 1], vec2(-1, 1));
        // Middle
        if (y > 0 && m_particleArray[x][y - 1] != nullptr)
            collidingObjects.emplace_back(
                m_particleArray[x][y - 1], vec2(0, -1));
        if (y < 499 && m_particleArray[x][y + 1] != nullptr)
            collidingObjects.emplace_back(
                m_particleArray[x][y + 1], vec2(0, 1));
        // Right Side
        if (x < 499 && y > 0 && m_particleArray[x + 1][y - 1] != nullptr)
            collidingObjects.emplace_back(
                m_particleArray[x + 1][y - 1], vec2(1, -1));
        if (x < 499 && m_particleArray[x + 1][y] != nullptr)
            collidingObjects.emplace_back(
                m_particleArray[x + 1][y], vec2(1, 0));
        if (x < 499 && y < 499 && m_particleArray[x + 1][y + 1] != nullptr)
            collidingObjects.emplace_back(
                m_particleArray[x + 1][y + 1], vec2(1, 1));

        const auto entity1Pointer = world.getEntity(entityHandle1);
        for (auto& [entity2, normal] : collidingObjects) {
            const auto& entityHandle2 = entity2->m_entityHandle;

            // If we hit, store the collision properties in a manifold
            world.makeComponent<CollisionManifoldComponent>(entityHandle1);
            auto manifoldComponent = static_cast<CollisionManifoldComponent*>(
                world.getComponent<CollisionManifoldComponent>(
                    *entity1Pointer));
            manifoldComponent->collisions.emplace_back(
                CollisionManifoldComponent::CollisionManifold{
                    world.getEntity(entityHandle2), normal, 0 });
        }
    }*/
}