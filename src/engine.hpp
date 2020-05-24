#pragma once
#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "Utility/vec.hpp"
#include "burningSystem.hpp"
#include "collisionCleanupSystem.hpp"
#include "collisionFinderSystem.hpp"
#include "collisionResolverSystem.hpp"
#include "combustionSystem.hpp"
#include "ecsWorld.hpp"
#include "entityCleanupSystem.hpp"
#include "gravitySystem.hpp"
#include "ignitionSystem.hpp"
#include "movementDetectorSystem.hpp"
#include "renderSystem.hpp"
#include "window.hpp"

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

    const Window& m_window;     ///< OS level window.
    double m_accumulator = 0.0; ///< Time left in the accumulator.
    ecsWorld m_gameWorld;       ///< The ECS world holding game state.
    MovementDetectorSystem m_moveDetector; ///< Cleans-up out of bounds.
    GravitySystem m_gravitySystem;         ///< System used to apply gravity.
    CollisionFinderSystem m_colFinder;     ///< Find collision events.
    CollisionResolverSystem m_colResolver; ///< Resolve collision events.
    IgnitionSystem m_igniter;              ///< Ignites flammable particles.
    BurningSystem m_burner;                ///< Burns on fire components.
    CombustionSystem m_combuster;          ///< Combusts explosive particles.
    EntityCleanupSystem m_cleanupSystem;   ///< Cleans-up out of bounds.
    CollisionCleanupSystem
        m_collisionCleanup; ///< System used to cleanup collision manifolds.
    RenderSystem m_renderSystem; ///< System used to render the game.
};

#endif // ENGINE_HPP