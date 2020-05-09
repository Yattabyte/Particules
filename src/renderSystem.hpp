#pragma once
#ifndef RENDERSYSTEM_HPP
#define RENDERSYSTEM_HPP

#include "Model/model.hpp"
#include "Multibuffer/glDynamicMultiBuffer.hpp"
#include "Utility/indirectDraw.hpp"
#include "Utility/shader.hpp"
#include "components.hpp"
#include "ecsSystem.hpp"

/***/
class RenderSystem final : public mini::ecsSystem {
    public:
    /***/
    RenderSystem();
    // Public Interface Implementation
    void updateComponents(
        const double& deltaTime,
        const std::vector<std::vector<mini::ecsBaseComponent*>>&
            entityComponents) final;

    private:
    // Private Attributes
    /** Particle rendering attributes. */
    Shader m_shader;
    Model m_model;
    IndirectDraw m_draw;
    glDynamicMultiBuffer<3> m_dataBuffer;
};

#endif // RENDERSYSTEM_HPP