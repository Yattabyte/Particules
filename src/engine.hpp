#pragma once
#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "CollisionManifoldSystem.hpp"
#include "CollisionSystem.hpp"
#include "Utility/vec.hpp"
#include "burningSystem.hpp"
#include "collisionCleanupSystem.hpp"
#include "combustionSystem.hpp"
#include "ecsWorld.hpp"
#include "entityCleanupSystem.hpp"
#include "ignitionSystem.hpp"
#include "renderSystem.hpp"
#include "spawnerSystem.hpp"
#include "window.hpp"
#include <array>

///////////////////////////////////////////////////////////////////////////
/// Use the shared mini namespace
using namespace mini;

/////////////////////////////////////////////////////////////////////////
/// \class  Engine
/// \brief  The core of the game-portion of the application.
class Engine {
    public:
    /////////////////////////////////////////////////////////////////////////
    /// \brief  Destroy the engine.
    ~Engine() = default;
    //////////////////////////////////////////////////////////////////////
    /// \brief  Deleted copy constructor.
    Engine(const Engine& o) = delete;
    //////////////////////////////////////////////////////////////////////
    /// \brief  Default move constructor.
    Engine(Engine&& o) noexcept = delete;
    /////////////////////////////////////////////////////////////////////////
    /// \brief  Construct an engine object using a specific window.
    explicit Engine(const Window& window);

    //////////////////////////////////////////////////////////////////////
    /// \brief  Deleted copy-assignment operator.
    Engine& operator=(const Engine&) = delete;
    //////////////////////////////////////////////////////////////////////
    /// \brief  Default move-assignment operator.
    Engine& operator=(Engine&&) noexcept = delete;

    /////////////////////////////////////////////////////////////////////////
    /// \brief  Tick the engine state. To be called externally by main loop.
    /// \param  deltaTime   the amount of time since last frame.
    void tick(const double& deltaTime);

    private:
    /////////////////////////////////////////////////////////////////////////
    /// \brief  Tick the game logic ahead by delta-time.
    /// \param  deltaTime   the amount of time since last frame.
    void gameTick(const double& deltaTime);
    /////////////////////////////////////////////////////////////////////////
    /// \brief  Tick the render logic ahead by delta-time.
    /// \param  deltaTime   the amount of time since last frame.
    void renderTick(const double& deltaTime);

    private:
    ///////////////////////////////////////////////////////////////////////////
    /// Private Members
    const Window& m_window;     ///< OS level window.
    double m_accumulator = 0.0; ///< Time left in the accumulator.
    std::array<ecsWorld, 64>
        m_gameWorlds;     ///< World divided into 64 pixel chunks
    ecsWorld m_gameWorld; ///< The ECS world holding game state.
    std::shared_ptr<ParticleComponent* [513][513]>
        m_particleArray;         ///< Array of particles
    CollisionSystem m_collision; ///< Sort and apply physics events
    CollisionManifoldSystem
        m_manifolds;               ///< Organize and apply collision manifolds.
    SpawnerSystem m_spawnerSystem; ///< Spawns a particle beneath it every tick.
    IgnitionSystem m_igniter;      ///< Ignites flammable particles.
    BurningSystem m_burner;        ///< Burns on fire components.
    CombustionSystem m_combuster;  ///< Combusts explosive particles.
    EntityCleanupSystem m_cleanupSystem; ///< Cleans-up out of bounds.
    CollisionCleanupSystem
        m_collisionCleanup; ///< System used to cleanup collision manifolds.
    RenderSystem m_renderSystem; ///< System used to render the game.
};

#endif // ENGINE_HPP