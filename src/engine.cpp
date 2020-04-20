#include "engine.hpp"
#include "GLFW/glfw3.h"
#include <algorithm>
#include <random>

constexpr auto const vertCode = R"END(
    #version 430
    struct Particle {
        vec2 pos;
        vec2 velocity;
        float mass;
    };

    layout (location = 0) in vec3 vertex;
    layout (location = 0) uniform mat4 pMatrix;
    layout (location = 4) uniform mat4 vMatrix;
    layout (std430, binding = 1) readonly buffer Particle_Buffer {
	    Particle particles[];
    };

    void main() {
        const vec3 offset = vec3(particles[gl_InstanceID].pos, 0.0);
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
      m_draw(4, 0, 0, GL_DYNAMIC_STORAGE_BIT) {
    std::uniform_real_distribution<float> randomFloats(-1.0F, 1.0F);
    std::mt19937 generator(0);
    m_particles.resize(256);
    std::generate(std::begin(m_particles), std::end(m_particles), [&]() {
        return Particle{ // Generate random position
                         vec2{ randomFloats(generator) * 32.0F,
                               randomFloats(generator) * 32.0F },
                         vec2(0.0), // Zero Velocity
                                    // Generate random mass
                         (randomFloats(generator) * 0.5F + 0.5F) * 4.5F + 0.5F
        };
    });
}

void Engine::tick(const double& deltaTime) {
    gameTick(deltaTime);
    renderTick(deltaTime);
}

void Engine::gameTick(const double& deltaTime) {
    constexpr double timeStep = 0.01;
    m_accumulator += deltaTime;
    while (m_accumulator > timeStep) {
        for (auto& particle : m_particles) {
            const float force = particle.m_mass * -9.81F;
            particle.m_velocity.y() += force * static_cast<float>(timeStep);

            particle.m_pos +=
                particle.m_velocity * vec2(static_cast<float>(timeStep));
        }
        m_accumulator -= timeStep;
    }
}

void Engine::renderTick(const double& /*deltaTime*/) {
    // Update buffered data
    m_draw.setPrimitiveCount(static_cast<GLuint>(m_particles.size()));
    m_dataBuffer.beginWriting();
    m_dataBuffer.write(
        0, sizeof(Particle) * m_particles.size(), m_particles.data());
    m_dataBuffer.endWriting();

    // Calculate viewing perspective and matrices
    const auto pMatrix = mat4::perspective(1.5708F, 1.0F, 0.01F, 10.0F);
    const auto vMatrix =
        mat4::lookAt(vec3{ 0, 0, 64 }, vec3{ 0, 0, 0 }, vec3{ 0, 1, 0 });

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