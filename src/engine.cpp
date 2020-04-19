#include "engine.hpp"
#include "GLFW/glfw3.h"

constexpr auto const vertCode = R"END(
    #version 430

    layout (location = 0) in vec3 vertex;
    layout (location = 0) uniform mat4 pMatrix;
    layout (location = 4) uniform mat4 vMatrix;
    layout (std430, binding = 1) readonly buffer Particle_Buffer {
	    vec2 pos[];
    };

    void main() {
        const vec3 offset = vec3(pos[gl_InstanceID], 0.0);
        gl_Position = pMatrix * vMatrix * vec4(vertex + offset,  1.0);
    }
)END";

constexpr auto const fragCode = R"END(
    #version 430

    layout (location = 0) out vec4 fragColor;
    layout (location = 8) uniform vec4 color;

    void main() {
        fragColor = color;
    }
)END";

Engine::Engine(const Window& window)
    : m_window(window), m_shader(vertCode, fragCode),
      m_model({ vec3(-0.5, -0.5, 0), vec3(0.5, -0.5, 0), vec3(0.5, 0.5, 0),
                vec3(-0.5, 0.5, 0) }),
      m_draw(4, 1, 0, GL_DYNAMIC_STORAGE_BIT) {}

void Engine::tick(const double& deltaTime) {
    gameTick(deltaTime);
    renderTick(deltaTime);
}

void Engine::gameTick(const double& /*deltaTime*/) {
    // Update Particles
    m_particles.resize(3);
    m_particles[0].m_pos =
        vec2(-2.0f, sinf(static_cast<float>(glfwGetTime()) * 2.0F));
    m_particles[1].m_pos =
        vec2(0, cosf(static_cast<float>(glfwGetTime()) * 2.0F));
    m_particles[2].m_pos =
        vec2(2.0f, -sinf(static_cast<float>(glfwGetTime()) * 2.0F));
}

void Engine::renderTick(const double& /*deltaTime*/) {
    // Update buffered data
    m_draw.setPrimitiveCount(static_cast<GLuint>(m_particles.size()));
    m_dataBuffer.beginWriting();
    m_dataBuffer.write(
        0, sizeof(Particle) * m_particles.size(), &m_particles[0]);
    m_dataBuffer.endWriting();

    // Calculate viewing perspective and matrices
    constexpr auto distance = 16.0F;
    constexpr auto math_pi = 3.14159F;
    const auto pMatrix = mat4::perspective(1.5708F, 1.0F, 0.01F, 10.0F);
    const auto vMatrix =
        mat4::lookAt(vec3{ 0, 0, 50 }, vec3{ 0, 0, 0 }, vec3{ 0, 1, 0 });

    // Flush buffers and set starting parameters
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_ONE, GL_ZERO);

    m_shader.uniformLocation(0, pMatrix);
    m_shader.uniformLocation(4, vMatrix);
    m_shader.uniformLocation(8, vec4{ 1.0 });
    m_shader.bind();
    m_model.bind();
    m_dataBuffer.bindBufferBase(GL_SHADER_STORAGE_BUFFER, 1);
    m_draw.drawCall(GL_QUADS);
    m_dataBuffer.endReading();
}