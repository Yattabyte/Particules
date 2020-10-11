#pragma once
#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "Utility/vec.hpp"
#include "definitions.hpp"
#include "physics.hpp"
#include "renderer.hpp"
#include "utilities.hpp"
#include "window.hpp"
#include <atomic>
#include <condition_variable>
#include <future>
#include <memory>
#include <shared_mutex>
#include <thread>

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
    void tick(const double deltaTime);
    void setMouseEvent(const MouseEvent& mouseEvent);

    private:
    ///////////////////////////////////////////////////////////////////////////
    /// Private Methods
    void inputTick(const double deltaTime);
    void gameTick(const double deltaTime);
    void gameTick_threaded(std::future<void> exitObject);
    void renderTick(const double deltaTime);

    ///////////////////////////////////////////////////////////////////////////
    /// Private Members
    const Window& m_window; ///< OS level window.
    ThreadStatus m_threadStatus;
    std::shared_mutex m_jobMutex;
    std::atomic<int> m_numJobsRemaining;
    std::vector<CellChunk> m_jobs;
    std::vector<std::tuple<std::thread, std::promise<void>>> m_threads;
    double m_gameAccumulator = 0.0; ///< Time left in the accumulator.
    std::shared_ptr<Particle[HEIGHT + 1][WIDTH + 1]> m_particles;
    Physics m_physics;
    Renderer m_renderer;
    MouseEvent m_mouseEvent;
    int m_tickNum = 0;
};

#endif // ENGINE_HPP