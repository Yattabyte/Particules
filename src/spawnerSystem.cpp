#include "spawnerSystem.hpp"
#include "collision.hpp"

//////////////////////////////////////////////////////////////////////
/// Custom Constructor
//////////////////////////////////////////////////////////////////////

SpawnerSystem::SpawnerSystem(
    ecsWorld& gameWorld,
    std::shared_ptr<ParticleComponent* [513][513]>& particleArray)
    : m_gameWorld(gameWorld), m_particleArray(particleArray) {
    addComponentType(ParticleComponent::Runtime_ID, RequirementsFlag::REQUIRED);
    addComponentType(SpawnerComponent::Runtime_ID, RequirementsFlag::REQUIRED);
}

//////////////////////////////////////////////////////////////////////
/// updateComponents
//////////////////////////////////////////////////////////////////////

void SpawnerSystem::updateComponents(
    const double& /*deltaTime*/,
    const std::vector<std::vector<ecsBaseComponent*>>& entityComponents) {
    constexpr std::array<float, 16> noise = {
        0.0774344872F, 0.0880199587F, 0.5709270134F, 0.2659348386F,
        0.6522730960F, 0.9039410920F, 0.0648135795F, 0.9500589706F,
        0.3770664946F, 0.2918298451F, 0.8389844759F, 0.1398756207F,
        0.7765582152F, 0.9703763453F, 0.2976069829F, 0.7423728685F
    };
    static int index = 0;

    for (const auto& components : entityComponents) {
        const auto& particleComponent =
            *static_cast<ParticleComponent*>(components[0]);
        const int x = static_cast<int>(particleComponent.m_pos.x());
        const int y = static_cast<int>(particleComponent.m_pos.y());

        const int newX = (x - 1) + static_cast<int>(noise[index] * 3.0F);
        index = ++index % 16;
        const int newY = (y - 1) + static_cast<int>(noise[index] * 2.0F);
        index = ++index % 16;
        if (newX > 0 && newX < 511 && newY > 0 && newY < 511) {
            if (m_particleArray[newY][newX] == nullptr) {
                ParticleComponent particle;
                particle.m_health = 10.0F;
                particle.m_density = 0.0F;
                particle.m_useGravity = true;
                particle.m_pos =
                    vec2(static_cast<float>(newX), static_cast<float>(newY));
                particle.m_color = COLOR_SAND;
                auto entityHandle = m_gameWorld.makeEntity();
                m_gameWorld.makeComponent(entityHandle, &particle);
            }
        }
    }
}