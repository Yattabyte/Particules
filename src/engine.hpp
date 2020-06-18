#pragma once
#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "Utility/vec.hpp"
#include "particle.hpp"
#include "physics.hpp"
#include "renderer.hpp"
#include "window.hpp"
#include <memory>

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
    ///////////////////////////////////////////////////////////////////////////
    /// Private Members
    const Window& m_window;     ///< OS level window.
    double m_accumulator = 0.0; ///< Time left in the accumulator.
    std::shared_ptr<Particle[769][769]> m_particles;
    Physics m_physics;
    Renderer m_renderer;
};

#endif // ENGINE_HPP