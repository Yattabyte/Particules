#pragma once
#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "Utility/vec.hpp"
#include "collisionSolver.hpp"
#include "ecsWorld.hpp"
#include "entityCleanupSystem.hpp"
#include "gravitySystem.hpp"
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
    /////////////////////////////////////////////////////////////////////////
    /// \brief  Construct an engine object using a specific window.
    explicit Engine(const Window& window);

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

    const Window& m_window;              ///< OS level window.
    double m_accumulator = 0.0;          ///< Time left in the accumulator.
    ecsWorld m_gameWorld;                ///< The ECS world holding game state.
    GravitySystem m_gravitySystem;       ///< System used to apply gravity.
    CollisionSolver m_collisionSolver;   ///< System used to collide entities.
    RenderSystem m_renderSystem;         ///< System used to render the game.
    EntityCleanupSystem m_cleanupSystem; ///< Cleans-up out of bounds.
};

#endif // ENGINE_HPP