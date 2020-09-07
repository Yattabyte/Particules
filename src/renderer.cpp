#include "renderer.hpp"

constexpr auto const vertCode = R"END(
    #version 430
    struct Particle {
        vec4 color;
        vec2 pos;
    };

    layout (location = 0) in vec3 vertex;
    layout (location = 0) uniform mat4 orthoMatrix;
    layout (std430, binding = 1) readonly buffer Particle_Buffer {
	    Particle particles[];
    };
    layout (location = 0) flat out vec4 color;

    void main() {
        const vec3 offset = vec3(particles[gl_InstanceID].pos, 0.0);
        gl_Position = orthoMatrix * vec4((vertex * 0.5) + offset,  1.0);
        color = particles[gl_InstanceID].color;
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

Renderer::Renderer(
    std::shared_ptr<Particle[HEIGHT + 1][WIDTH + 1]>& particles) noexcept
    : m_shader(vertCode, fragCode), m_model({ vec3(-1, -1, 0), vec3(1, -1, 0),
                                              vec3(1, 1, 0), vec3(-1, 1, 0) }),
      m_draw(4, 0, 0, GL_DYNAMIC_STORAGE_BIT), m_particles(particles) {
    const mat4 orthoMatrix = mat4::orthographic(
        -1.0F, static_cast<float>(WIDTH - 1), static_cast<float>(HEIGHT - 1),
        -1.0F, -1.0F, 1.0F);
    m_shader.uniformLocation(0, orthoMatrix);
}

//////////////////////////////////////////////////////////////////////
/// draw
//////////////////////////////////////////////////////////////////////

void Renderer::draw(const double /*deltaTime*/) noexcept {
    // Update buffered data
    m_dataBuffer.beginWriting();
    size_t offset(0ULL);
    unsigned int count(0);
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            const auto& particle = m_particles[y][x];
            // if (particle.m_element == Element::AIR)
            //    continue;

            /*// Convert game particles into GPU renderable particles
            const auto value =
                (particle.m_temp / 100.0F) * 3.14159F / 2.0F;
            vec4 color(
                std::sin(value), std::sin(value * 2), std::cos(value), 1.0);*/

            //vec4 color(std::clamp(particle.m_temp / 100.0F, 0.0F, 1.0F));
            const GPU_Particle data{
                (particle.m_attributes & Attributes::ON_FIRE) !=
                        Attributes::ON_FIRE
                    ? COLORS[static_cast<int>(particle.m_element)]
                    : COLORS[static_cast<int>(Element::FIRE)],
                //color,
                vec2(static_cast<float>(x), static_cast<float>(y)),
            };
            m_dataBuffer.write(offset, sizeof(GPU_Particle), &data);
            offset += sizeof(GPU_Particle);
            ++count;
        }
    }
    m_dataBuffer.endWriting();
    m_draw.setPrimitiveCount(static_cast<GLuint>(count));

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