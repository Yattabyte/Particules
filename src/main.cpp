#define GLFW_INCLUDE_NONE
#include "Model/model.hpp"
#include "Utility/indirectDraw.hpp"
#include "Utility/shader.hpp"
#include "Utility/vec.hpp"
#include "Utility/window.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <glad/glad.h>
#include <iostream>
#include <random>
#include <vector>

constexpr auto const vertCode = R"END(
    #version 430

    layout (location = 0) in vec3 vertex;
    layout (location = 0) uniform mat4 pMatrix;
    layout (location = 4) uniform mat4 vMatrix;
    layout (location = 8) uniform mat4 mMatrix;

    void main() {
        gl_Position = pMatrix * vMatrix * mMatrix * vec4(vertex, 1.0);
        gl_PointSize = 10.0;
    }
)END";

constexpr auto const fragCode = R"END(
    #version 430

    layout (location = 0) out vec4 fragColor;
    layout (location = 12) uniform vec4 color;

    void main() {
        fragColor = color;
    }
)END";

/** Report an error and shutdown. */
void error_shutdown(const std::string& errorMsg) {
    std::cout << errorMsg;
    glfwTerminate();
    exit(-1);
}

void register_debug() {
#ifdef DEBUG
    if (GLAD_GL_KHR_debug != 0) {
        GLint v;
        glGetIntegerv(GL_CONTEXT_FLAGS, &v);
        if ((v != 0) && GL_CONTEXT_FLAG_DEBUG_BIT) {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            constexpr const static auto myCallback =
                [](GLenum source, GLenum type, GLuint id, GLenum severity,
                   GLsizei length, const GLchar* msg, const void* /*unused*/) {
                    std::string _source;
                    std::string _type;
                    std::string _severity;
                    switch (source) {
                    case GL_DEBUG_SOURCE_API:
                        _source = "API";
                        break;
                    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
                        _source = "WINDOW SYSTEM";
                        break;
                    case GL_DEBUG_SOURCE_SHADER_COMPILER:
                        _source = "SHADER COMPILER";
                        break;
                    case GL_DEBUG_SOURCE_THIRD_PARTY:
                        _source = "THIRD PARTY";
                        break;
                    case GL_DEBUG_SOURCE_APPLICATION:
                        _source = "APPLICATION";
                        break;
                    case GL_DEBUG_SOURCE_OTHER:
                        _source = "OTHER";
                        break;
                    default:
                        _source = "UNKNOWN";
                        break;
                    }

                    switch (type) {
                    case GL_DEBUG_TYPE_ERROR:
                        _type = "ERROR";
                        break;
                    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                        _type = "DEPRECATED BEHAVIOR";
                        break;
                    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                        _type = "UDEFINED BEHAVIOR";
                        break;
                    case GL_DEBUG_TYPE_PORTABILITY:
                        _type = "PORTABILITY";
                        break;
                    case GL_DEBUG_TYPE_PERFORMANCE:
                        _type = "PERFORMANCE";
                        break;
                    case GL_DEBUG_TYPE_OTHER:
                        _type = "OTHER";
                        break;
                    case GL_DEBUG_TYPE_MARKER:
                        _type = "MARKER";
                        break;
                    default:
                        _type = "UNKNOWN";
                        break;
                    }

                    switch (severity) {
                    case GL_DEBUG_SEVERITY_HIGH:
                        _severity = "HIGH";
                        break;
                    case GL_DEBUG_SEVERITY_MEDIUM:
                        _severity = "MEDIUM";
                        break;
                    case GL_DEBUG_SEVERITY_LOW:
                        _severity = "LOW";
                        break;
                    case GL_DEBUG_SEVERITY_NOTIFICATION:
                        _severity = "NOTIFICATION";
                        break;
                    default:
                        _severity = "UNKNOWN";
                        break;
                    }
                    if (severity != GL_DEBUG_SEVERITY_NOTIFICATION &&
                        severity != GL_DEBUG_SEVERITY_LOW)
                        std::cout
                            << std::to_string(id) << ": " << _type << " of "
                            << _severity << " severity, raised from " << _source
                            << ": " << std::string(msg, length) << std::endl;
                };
            glDebugMessageCallbackKHR(myCallback, nullptr);
        }
    }
#endif
}

int main() {
    // Init GLFW
    if (glfwInit() != GLFW_TRUE)
        error_shutdown("Failed to initialize GLFW\n");

    // Create Window
    const Window window(512, 512);
    if (!window.exists())
        error_shutdown("Failed to create a window.\n");

    // Init GL functions
    glfwMakeContextCurrent(window.pointer());
    if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) ==
        0)
        error_shutdown("Failed to initialize OpenGL context.\n");
    register_debug();

    // Create objects within scope
    {
        // Make shaders
        const Shader shader(vertCode, fragCode);
        if (!shader.valid())
            error_shutdown(shader.errorLog());

        // Enable point rendering and blending
        glEnable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glLineWidth(4.0F);

        // Main Loop
        double lastTime(0.0);
        while (glfwWindowShouldClose(window.pointer()) == 0) {
            const auto time = glfwGetTime();
            const auto deltaTime = time - lastTime;
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            lastTime = time;
            glfwPollEvents();
            glfwSwapBuffers(window.pointer());
        }
    }

    // Success
    glfwTerminate();
    return 0;
}