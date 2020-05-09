#pragma once
#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "Utility/vec.hpp"
#include "Utility/window.hpp"
#include "collisionSystem.hpp"
#include "ecsWorld.hpp"
#include "gravitySystem.hpp"
#include "renderSystem.hpp"

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

    const Window& m_window;            ///< OS level window.
    double m_accumulator = 0.0;        ///< Time left in the accumulator.
    mini::ecsWorld m_gameWorld;        ///< The ECS world holding game state.
    CollisionSystem m_collisionSystem; ///< System used to collide entities.
    GravitySystem m_gravitySystem;     ///< System used to apply gravity.
    RenderSystem m_renderSystem;       ///< System used to render the game.
};

#endif // ENGINE_HPP