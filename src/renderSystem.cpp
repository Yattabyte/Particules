#include "renderSystem.hpp"

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

RenderSystem::RenderSystem()
    : m_shader(vertCode, fragCode), m_model({ vec3(-1, -1, 0), vec3(1, -1, 0),
                                              vec3(1, 1, 0), vec3(-1, 1, 0) }),
      m_draw(4, 0, 0, GL_DYNAMIC_STORAGE_BIT) {
    addComponentType(
        ParticleComponent::Runtime_ID, RequirementsFlag::FLAG_REQUIRED);
    addComponentType(
        BoundingSphereComponent::Runtime_ID, RequirementsFlag::FLAG_OPTIONAL);
    addComponentType(
        BoundingBoxComponent::Runtime_ID, RequirementsFlag::FLAG_OPTIONAL);
}

void RenderSystem::updateComponents(
    const double& /*deltaTime*/,
    const std::vector<std::vector<ecsBaseComponent*>>& entityComponents) {
    // Update buffered data
    m_draw.setPrimitiveCount(static_cast<GLuint>(entityComponents.size()));
    m_dataBuffer.beginWriting();

    size_t offset(0ULL);
    for (const auto& components : entityComponents) {
        // Convert game particles into GPU renderable particles
        const auto& particle =
            dynamic_cast<ParticleComponent*>(components[0])->particle;
        GPU_Particle data{ particle.m_pos, particle.m_velocity,
                           vec2(particle.m_size) };

        // For now, attempt to retrieve a scale based on whatever other
        // components are present
        if (const auto& bsphere =
                dynamic_cast<BoundingSphereComponent*>(components[1]))
            data.m_scale = vec2(bsphere->radius);
        else if (
            const auto& bbox =
                dynamic_cast<BoundingBoxComponent*>(components[2]))
            data.m_scale = bbox->extents;

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