#pragma once
#ifndef ENTITYCLEANUPSYSTEM_HPP
#define ENTITYCLEANUPSYSTEM_HPP

#include "components.hpp"
#include "ecsSystem.hpp"

///////////////////////////////////////////////////////////////////////////
/// Use the shared mini namespace
using namespace mini;

/////////////////////////////////////////////////////////////////////////
/// \class  EntityCleanupSystem
/// \brief  System used to delete out-of-bound entities.
class EntityCleanupSystem final : public ecsSystem {
    public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Construct a cleanup system.
    EntityCleanupSystem(ecsWorld& gameWorld) : m_gameWorld(gameWorld) {
        addComponentType(
            ParticleComponent::Runtime_ID, RequirementsFlag::FLAG_REQUIRED);
        addComponentType(
            BoundingBoxComponent::Runtime_ID, RequirementsFlag::FLAG_REQUIRED);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Tick this system by deltaTime.
    /// \param	deltaTime	    the amount of time passed since last update.
    /// \param	components	    the components to update.
    void updateComponents(
        const double&,
        const std::vector<std::vector<ecsBaseComponent*>>& entityComponents)
        final {
        std::vector<EntityHandle> entitiesToDelete;
        for (auto& components : entityComponents) {
            const auto& particleComponent =
                *static_cast<ParticleComponent*>(components[0]);
            const auto& boundingComponent =
                *static_cast<BoundingBoxComponent*>(components[1]);
            const auto& position = particleComponent.particle.m_pos;
            const auto& extents = boundingComponent.extents;

            if (position.x() - extents.x() > 250.0F ||
                position.x() + extents.x() < -250.0F ||
                position.y() - extents.y() > 250.0F ||
                position.y() + extents.y() < -250.0F)
                entitiesToDelete.emplace_back(particleComponent.m_entityHandle);
        }

        for (const auto& handle : entitiesToDelete)
            m_gameWorld.removeEntity(handle);
    }

    private:
    ecsWorld& m_gameWorld;
};

#endif // ENTITYCLEANUPSYSTEM_HPP