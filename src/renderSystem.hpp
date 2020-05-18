#pragma once
#ifndef RENDERSYSTEM_HPP
#define RENDERSYSTEM_HPP

#include "Model/model.hpp"
#include "Multibuffer/glDynamicMultiBuffer.hpp"
#include "Utility/indirectDraw.hpp"
#include "Utility/shader.hpp"
#include "components.hpp"
#include "ecsSystem.hpp"

///////////////////////////////////////////////////////////////////////////
/// Use the shared mini namespace
using namespace mini;

/////////////////////////////////////////////////////////////////////////
/// \class  RenderSystem
/// \brief  System used to render entities to the screen.
class RenderSystem final : public ecsSystem {
    public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Construct a rendering system.
    RenderSystem();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Tick this system by deltaTime.
    /// \param	deltaTime	    the amount of time passed since last update.
    /// \param	components	    the components to update.
    void updateComponents(
        const double& deltaTime,
        const std::vector<std::vector<ecsBaseComponent*>>& entityComponents)
        final;

    private:
    Shader m_shader;                      ///< A shader for displaying particles
    Model m_model;                        ///< A model for particles
    IndirectDraw m_draw;                  ///< An indirect draw call GL object
    glDynamicMultiBuffer<3> m_dataBuffer; ///< GPU data container
    std::vector<GPU_Particle> m_scratchBuf; ///< Scratch buffer for memory ops.
};

#endif // RENDERSYSTEM_HPP