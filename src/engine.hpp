#pragma once
#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "Model/model.hpp"
#include "Multibuffer/glDynamicMultiBuffer.hpp"
#include "Utility/indirectDraw.hpp"
#include "Utility/shader.hpp"
#include "Utility/vec.hpp"
#include "Utility/window.hpp"
#include <vector>

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
    const Window& m_window;
    Shader m_shader;
    Model m_model;
    IndirectDraw m_draw;
    struct Particle {
        vec2 m_pos = vec2(0.0F);
        vec2 m_velocity = vec2(0.0F);
        float m_mass = 1.0F;
        [[maybe_unused]] float padding;
    };
    double m_accumulator = 0.0;
    std::vector<Particle> m_particles;
    glDynamicMultiBuffer<3> m_dataBuffer;
};

#endif // ENGINE_HPP