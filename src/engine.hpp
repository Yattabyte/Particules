#pragma once
#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "Utility/vec.hpp"
#include "Utility/window.hpp"
#include "collisionSystem.hpp"
#include "ecsWorld.hpp"
#include "gravitySystem.hpp"
#include "renderSystem.hpp"

/***/
class Engine {
    public:
    // Public (De)Constructors
    /** Destroy the engine. */
    ~Engine() = default;
    /** Construct an engine object using a specific window. */
    explicit Engine(const Window& window);

    // Public Methods
    /** Tick the engine state. To be called externally by main loop. */
    void tick(const double& deltaTime);

    private:
    /***/
    void gameTick(const double& deltaTime);
    /***/
    void renderTick(const double& deltaTime);

    // Private Attributes
    /** OS level winding attributes. */
    const Window& m_window;
    /** Game-play related attributes.*/
    double m_accumulator = 0.0;
    mini::ecsWorld m_gameWorld;
    CollisionSystem m_collisionSystem;
    GravitySystem m_gravitySystem;
    RenderSystem m_renderSystem;
};

#endif // ENGINE_HPP