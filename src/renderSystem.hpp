#pragma once
#ifndef RENDERSYSTEM_HPP
#define RENDERSYSTEM_HPP

#include "Model/model.hpp"
#include "Multibuffer/glDynamicMultiBuffer.hpp"
#include "Utility/indirectDraw.hpp"
#include "Utility/shader.hpp"
#include "components.hpp"
#include "ecsSystem.hpp"

/////////////////////////////////////////////////////////////////////////
/// \class  RenderSystem
/// \brief  System used to render entities to the screen.
class RenderSystem final : public mini::ecsSystem {
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
        const std::vector<std::vector<mini::ecsBaseComponent*>>&
            entityComponents) final;

    private:
    Shader m_shader;                      ///< A shader for displaying particles
    Model m_model;                        ///< A model for particles
    IndirectDraw m_draw;                  ///< An indirect draw call GL object
    glDynamicMultiBuffer<3> m_dataBuffer; ///<  GPU data container
};

#endif // RENDERSYSTEM_HPP