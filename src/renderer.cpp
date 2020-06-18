#include "renderer.hpp"

constexpr auto const vertCode = R"END(
    #version 430
    struct Particle {
        vec4 color;
        vec2 pos;
        int onFire;
    };

    layout (location = 0) in vec3 vertex;
    layout (location = 0) uniform mat4 pMatrix;
    layout (location = 4) uniform mat4 vMatrix;
    layout (std430, binding = 1) readonly buffer Particle_Buffer {
	    Particle particles[];
    };
    layout (location = 0) flat out vec4 color;

    void main() {
        const vec3 offset = vec3(particles[gl_InstanceID].pos, 0.0);
        gl_Position = pMatrix * vMatrix * vec4((vertex * 0.5) + offset,  1.0);
        color = mix(particles[gl_InstanceID].color, vec4(1, 0.2F, 0, 1), particles[gl_InstanceID].onFire * 0.75);
    }
)END";

constexpr auto const fragCode = R"END(
    #version 430

    layout (location = 0) flat in vec4 color;
    layout (location = 0) out vec4 fragColor;

    void main() {
        fragColor = color;
    }
)END";

//////////////////////////////////////////////////////////////////////
/// Custom Constructor
//////////////////////////////////////////////////////////////////////

Renderer::Renderer(std::shared_ptr<Particle[769][769]>& particles)
    : m_shader(vertCode, fragCode), m_model({ vec3(-1, -1, 0), vec3(1, -1, 0),
                                              vec3(1, 1, 0), vec3(-1, 1, 0) }),
      m_draw(4, 0, 0, GL_DYNAMIC_STORAGE_BIT), m_particles(particles) {
    // Calculate viewing perspective and matrices
    const auto pMatrix = mat4::perspective(1.5708F, 1.0F, 0.01F, 10.0F);
    const auto vMatrix = mat4::lookAt(
        vec3{ 384, 384, 384 }, vec3{ 384, 384, 0 }, vec3{ 0, 1, 0 });

    m_shader.uniformLocation(0, pMatrix);
    m_shader.uniformLocation(4, vMatrix);
}

//////////////////////////////////////////////////////////////////////
/// updateComponents
//////////////////////////////////////////////////////////////////////

void Renderer::draw(const double& /*deltaTime*/) {
    // Update buffered data
    m_draw.setPrimitiveCount(static_cast<GLuint>(768 * 768));
    m_dataBuffer.beginWriting();
    size_t offset(0ULL);
    for (int y = 0; y < 768; ++y) {
        for (int x = 0; x < 768; ++x) {
            const auto& particle = m_particles[y][x];
            // Convert game particles into GPU renderable particles
            const GPU_Particle data{
                particle.m_color,
                vec2(static_cast<float>(x), static_cast<float>(y)),
                particle.m_onFire,
            };
            m_dataBuffer.write(offset, sizeof(GPU_Particle), &data);
            offset += sizeof(GPU_Particle);
        }
    }
    m_dataBuffer.endWriting();

    // Flush buffers and set starting parameters
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_ONE, GL_ZERO);

    m_shader.bind();
    m_model.bind();
    m_dataBuffer.bindBufferBase(GL_SHADER_STORAGE_BUFFER, 1);
    m_draw.drawCall(GL_QUADS);
    m_dataBuffer.endReading();
}