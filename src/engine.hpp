#pragma once
#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "Utility/window.hpp"

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
    // Private Attributes
    const Window& m_window;
};

#endif // ENGINE_HPP