#include "collisionSystem.hpp"
#include <array>

//////////////////////////////////////////////////////////////////////
/// Custom Constructor
//////////////////////////////////////////////////////////////////////

CollisionSystem::CollisionSystem(
    ecsWorld& gameWorld,
    std::shared_ptr<ParticleComponent* [513][513]>& particleArray)
    : m_gameWorld(gameWorld), m_particleArray(particleArray) {
    addComponentType(ParticleComponent::Runtime_ID, RequirementsFlag::REQUIRED);
}

//////////////////////////////////////////////////////////////////////
/// findCollisions
//////////////////////////////////////////////////////////////////////

void CollisionSystem::updateComponents(
    const double& /*deltaTime*/,
    const std::vector<std::vector<ecsBaseComponent*>>& entityComponents) {
    for (const auto& components : entityComponents) {
        auto& particleComponent =
            *static_cast<ParticleComponent*>(components.front());
        const int x = static_cast<int>(particleComponent.m_pos.x());
        const int y = static_cast<int>(particleComponent.m_pos.y());
        m_particleArray[y][x] = &particleComponent;
    }

    // Apply Gravity
    for (int y = 0; y < 512; ++y) {
        for (int x = 0; x < 512; ++x) {
            auto& particle1 = m_particleArray[y][x];

            // Only act on particles that can move
            if (particle1 == nullptr || particle1->m_asleep ||
                !particle1->m_useGravity)
                continue;

            // Avoid else branch set to true early
            particle1->m_asleep = true;

            const auto swapTile = [&](const int& newX, const int& newY) {
                // Set new position
                particle1->m_pos =
                    vec2(static_cast<float>(newX), static_cast<float>(newY));
                particle1->m_asleep = false;
                // Swap tiles in array
                std::swap(m_particleArray[y][x], m_particleArray[newY][newX]);
                // Wake up above particle
                if (m_particleArray[y + 1][x] != nullptr)
                    m_particleArray[y + 1][x]->m_asleep = false;
            };

            // Check if bottom is free
            if (m_particleArray[y - 1][x] == nullptr)
                swapTile(x, y - 1);
            // Check if bottom left is free
            else if (m_particleArray[y - 1][x - 1] == nullptr)
                swapTile(x - 1, y - 1);
            // Check if bottom right is free
            else if (m_particleArray[y - 1][x + 1] == nullptr)
                swapTile(x + 1, y - 1);
        }
    }
}