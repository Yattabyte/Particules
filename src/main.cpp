#define GLFW_INCLUDE_NONE
#include "Utility/vec.hpp"
#include "definitions.hpp"
#include "engine.hpp"
#include "window.hpp"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <string>

//////////////////////////////////////////////////////////////////////
/// Forward Declarations
static Window init_backend(const vec2& windowSize);
static void error_shutdown(const std::string& errorMsg);
static void register_debug() noexcept;

//////////////////////////////////////////////////////////////////////
/// main
//////////////////////////////////////////////////////////////////////

int main() noexcept {
    const Window window = init_backend(vec2(WIDTH, HEIGHT));
    Engine engine(window);

    // Main Loop
    double lastTime(0.0);
    while (glfwWindowShouldClose(window.pointer()) == 0) {
        const auto time = glfwGetTime();
        const auto deltaTime = time - lastTime;

        // Engine Tick
        engine.tick(deltaTime);

        lastTime = time;
        glfwPollEvents();
        glfwSwapBuffers(window.pointer());
    }

    // Success
    glfwTerminate();
    exit(0);
}

//////////////////////////////////////////////////////////////////////
/// init_backend
//////////////////////////////////////////////////////////////////////

static Window init_backend(const vec2& windowSize) {
    // Init GLFW
    if (glfwInit() != GLFW_TRUE)
        error_shutdown("Failed to initialize GLFW\n");

    // Create Window
    Window window(
        static_cast<int>(windowSize.x()), static_cast<int>(windowSize.y()));
    if (!window.exists())
        error_shutdown("Failed to create a window.\n");

    // Init GL functions
    glfwMakeContextCurrent(window.pointer());
    if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) ==
        0)
        error_shutdown("Failed to initialize OpenGL context.\n");
    register_debug();

    return window;
}

//////////////////////////////////////////////////////////////////////
/// error_shutdown
//////////////////////////////////////////////////////////////////////

static void error_shutdown(const std::string& errorMsg) {
    std::cout << errorMsg;
    glfwTerminate();
    exit(-1);
}

//////////////////////////////////////////////////////////////////////
/// register_debug
//////////////////////////////////////////////////////////////////////

static void register_debug() noexcept {
#ifdef DEBUG
    if (GLAD_GL_KHR_debug != 0) {
        GLint v(0);
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