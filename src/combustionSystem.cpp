#include "combustionSystem.hpp"

//////////////////////////////////////////////////////////////////////
/// Custom Constructor
//////////////////////////////////////////////////////////////////////

CombustionSystem::CombustionSystem(
    ecsWorld& gameWorld,
    std::shared_ptr<ParticleComponent* [513][513]>& particleArray)
    : m_gameWorld(gameWorld), m_particleArray(particleArray) {
    addComponentType(ParticleComponent::Runtime_ID, RequirementsFlag::REQUIRED);
    addComponentType(
        ExplosiveComponent::Runtime_ID, RequirementsFlag::REQUIRED);
    addComponentType(OnFireComponent::Runtime_ID, RequirementsFlag::REQUIRED);
    addComponentType(
        CollisionManifoldComponent::Runtime_ID, RequirementsFlag::OPTIONAL);
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
        auto collisionComponent =
            dynamic_cast<CollisionManifoldComponent*>(components[2]);
        const auto dt = static_cast<float>(deltaTime);
        const int x = static_cast<int>(particleComponent.m_pos.x());
        const int y = static_cast<int>(particleComponent.m_pos.y());

        // Only detonate entities whose fuse ran out
        if (explosiveComponent.fuseTime > 0.0001F) {
            explosiveComponent.fuseTime -= dt;
            continue;
        }

        if (collisionComponent != nullptr) {
            auto entityPointer1 =
                m_gameWorld.getEntity(particleComponent.m_entityHandle);
            for ([[maybe_unused]] auto& collisionEntry :
                 collisionComponent->collisions) {
                ///\todo apply high pressure point at this position

                // Set targets within radius on fire
                if (m_gameWorld.getComponent<FlammableComponent>(
                        *entityPointer1)) {
                    m_gameWorld.makeComponent<OnFireComponent>(*entityPointer1);
                }
            }
        }
        particleComponent.m_color = COLOR_SLUDGE;
        entitiesToBecomeInnert.emplace_back(particleComponent.m_entityHandle);
    }

    // Remove all identified entities
    for (const auto& handle : entitiesToBecomeInnert)
        m_gameWorld.removeComponent<ExplosiveComponent>(handle);
}