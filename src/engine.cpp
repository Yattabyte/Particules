#include "engine.hpp"
#include "GLFW/glfw3.h"
#include "collision.hpp"
#include <algorithm>
#include <random>

constexpr auto const vertCode = R"END(
    #version 430
    struct Particle {
        vec2 pos;
        vec2 velocity;
        vec2 scale;
    };

    layout (location = 0) in vec3 vertex;
    layout (location = 0) uniform mat4 pMatrix;
    layout (location = 4) uniform mat4 vMatrix;
    layout (std430, binding = 1) readonly buffer Particle_Buffer {
	    Particle particles[];
    };

    void main() {
        const vec3 scale = vec3(particles[gl_InstanceID].scale, 1.0);
        const vec3 offset = vec3(particles[gl_InstanceID].pos, 0.0);
        gl_Position = pMatrix * vMatrix * vec4((vertex * scale) + offset,  1.0);
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
      m_model(
          { vec3(-1, -1, 0), vec3(1, -1, 0), vec3(1, 1, 0), vec3(-1, 1, 0) }),
      m_draw(4, 0, 0, GL_DYNAMIC_STORAGE_BIT) {
    std::uniform_real_distribution<float> randomFloats(-1.0F, 1.0F);
    std::mt19937 generator(0);
    m_gameParticles.resize(1024);
    std::generate(
        std::begin(m_gameParticles), std::end(m_gameParticles), [&]() {
            return Game_Particle{
                // Generate random position
                vec2{ randomFloats(generator) * 32.0F,
                      randomFloats(generator) * 32.0F },
                // Zero Velocity
                vec2(0.0), PARTICLE_TYPE::SAND,
                // Generate random mass
                (randomFloats(generator) * 0.5F + 0.5F) * 49.5F + 0.5F, 1.0F
            };
        });
    m_gameParticles.push_back(Game_Particle{
        vec2{ 0, -50 }, vec2(0.0), PARTICLE_TYPE::CONCRETE, 0.0F, 10.0F });
}

void Engine::tick(const double& deltaTime) {
    gameTick(deltaTime);
    renderTick(deltaTime);
}

void Engine::gameTick(const double& deltaTime) {
    constexpr double timeStep = 0.01;
    m_accumulator += deltaTime;
    while (m_accumulator > timeStep) {
        // Apply external forces (e.g. gravity)
        for (auto& particle : m_gameParticles) {
            const float force = particle.m_mass * -9.81F;
            particle.m_velocity.y() += force * static_cast<float>(timeStep);

            particle.m_pos +=
                particle.m_velocity * vec2(static_cast<float>(timeStep));
        }

        // Adjust for collision
        for (auto& particle : m_gameParticles) {
            // Ignore massless particles
            if (particle.m_mass < 0.001F)
                continue;
            // Ignore immovable particles
            else if (particle.m_type == PARTICLE_TYPE::CONCRETE)
                continue;
            // Adjust physics interact-able particles
            else if (particle.m_type == PARTICLE_TYPE::SAND) {
                for (auto& otherParticle : m_gameParticles) {
                    if (otherParticle.m_type == PARTICLE_TYPE::CONCRETE) {
                        if (areColliding_SphereVsBox(
                                particle.m_pos, 1.0F, otherParticle.m_pos,
                                vec2(otherParticle.m_size))) {
                            // Find collision normal
                            const vec2 collisionNormal = vec2(0, 1);

                            // Find the intersection point
                            const vec2 intersectionPoint = lineLineIntersection(
                                particle.m_pos - particle.m_velocity,
                                particle.m_pos + particle.m_velocity,
                                otherParticle.m_pos + vec2(
                                                          -otherParticle.m_size,
                                                          otherParticle.m_size),
                                otherParticle.m_pos +
                                    vec2(otherParticle.m_size));

                            // Retract movement to intersection point
                            // (offset by radius of particle)
                            particle.m_pos = intersectionPoint +
                                             vec2(1.0F) * collisionNormal;

                            // Reflect the velocity off of the surface
                            particle.m_velocity =
                                (particle.m_velocity -
                                 vec2(2.0F) *
                                     vec2(particle.m_velocity.dot(
                                         collisionNormal)) *
                                     collisionNormal) *
                                vec2(0.5F);
                        }
                    }
                }
            }
        }
        m_accumulator -= timeStep;
    }
}

void Engine::renderTick(const double& /*deltaTime*/) {
    // Update buffered data
    m_draw.setPrimitiveCount(static_cast<GLuint>(m_gameParticles.size()));
    m_dataBuffer.beginWriting();
    size_t offset(0ULL);
    for (const auto& particle : m_gameParticles) {
        // Convert game particles into GPU renderable particles
        const GPU_Particle data{ particle.m_pos, particle.m_velocity,
                                 vec2(particle.m_size) };
        m_dataBuffer.write(offset, sizeof(GPU_Particle), &data);
        offset += sizeof(GPU_Particle);
    }
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