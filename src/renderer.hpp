#pragma once
#ifndef Renderer_HPP
#define Renderer_HPP

#include "Model/model.hpp"
#include "Multibuffer/glDynamicMultiBuffer.hpp"
#include "Utility/indirectDraw.hpp"
#include "Utility/shader.hpp"
#include "definitions.hpp"

///////////////////////////////////////////////////////////////////////////
/// Use the shared mini namespace
using namespace mini;

/////////////////////////////////////////////////////////////////////////
/// \class  Renderer
/// \brief  System used to render particles to the screen.
class Renderer {
    public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Construct a rendering system.
    /// \param  particles   structure identifying particles spatially.
    explicit Renderer(
        std::shared_ptr<Particle[HEIGHT + 1][WIDTH + 1]>& particles) noexcept;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Tick this system by deltaTime.
    /// \param	deltaTime	    the amount of time passed since last update.
    void draw(const double& deltaTime) noexcept;

    private:
    Shader m_shader;                      ///< A shader for displaying particles
    Model m_model;                        ///< A model for particles
    IndirectDraw m_draw;                  ///< An indirect draw call GL object
    glDynamicMultiBuffer<3> m_dataBuffer; ///< GPU data container
    std::shared_ptr<Particle[HEIGHT + 1][WIDTH + 1]>&
        m_particles; ///< Array of particles
};

#endif // Renderer_HPP