#include "Graphics.h"
#include "Framework.h"
#include <stdio.h>
#include <string>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <iostream>
#include <unordered_map>

#if defined(__ANDROID__) || defined(ANDROID)
#include <android/sensor.h>
#include <EGL/egl.h>
#else
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#endif

namespace wuh {

#ifndef ANDROID
static void GLAPIENTRY errorOccurredGL(
        GLenum source, GLenum type, GLuint id, GLenum severity,
        GLsizei length, const GLchar *message, const void *userParam)
{
    (void)length;
    (void)userParam;

    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
        return;

    const char* source_str;
    const char* type_str;
    const char* severity_str;

    switch (source) {
        case GL_DEBUG_SOURCE_API:
            source_str = "API";
            break;

        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            source_str = "WINDOW SYSTEM";
            break;

        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            source_str = "SHADER COMPILER";
            break;

        case GL_DEBUG_SOURCE_THIRD_PARTY:
            source_str = "THIRD PARTY";
            break;

        case GL_DEBUG_SOURCE_APPLICATION:
            source_str = "APPLICATION";
            break;

        case GL_DEBUG_SOURCE_OTHER:
            source_str = "UNKNOWN";
            break;

        default:
            source_str = "UNKNOWN";
            break;
    }

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            type_str = "ERROR";
            break;

        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            type_str = "DEPRECATED BEHAVIOR";
            break;

        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            type_str = "UDEFINED BEHAVIOR";
            break;

        case GL_DEBUG_TYPE_PORTABILITY:
            type_str = "PORTABILITY";
            break;

        case GL_DEBUG_TYPE_PERFORMANCE:
            type_str = "PERFORMANCE";
            break;

        case GL_DEBUG_TYPE_OTHER:
            type_str = "OTHER";
            break;

        case GL_DEBUG_TYPE_MARKER:
            type_str = "MARKER";
            break;

        default:
            type_str = "UNKNOWN";
            break;
    }

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            severity_str = "HIGH";
            break;

        case GL_DEBUG_SEVERITY_MEDIUM:
            severity_str = "MEDIUM";
            break;

        case GL_DEBUG_SEVERITY_LOW:
            severity_str = "LOW";
            break;

        case GL_DEBUG_SEVERITY_NOTIFICATION:
            severity_str = "NOTIFICATION";
            break;

        default:
            severity_str = "UNKNOWN";
            break;
    }
    
    std::stringstream ss;
    ss  << "Message from OpenGL: source: " << source_str
        << ", type: " << type_str
        << ", id: 0x" << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << id
        << " , severity: " << severity_str << "\n"
        << message << "\n";

    if (severity == GL_DEBUG_SEVERITY_HIGH) {
        throw std::runtime_error(ss.str());
    } else {
        std::cout << ss.rdbuf();
    }
}
#endif

Graphics& Graphics::init() {

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

#ifndef __EMSCRIPTEN__
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

#if !defined(__ANDROID__) && !defined(ANDROID) && !defined(__EMSCRIPTEN__) && !defined(__APPLE__)
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(errorOccurredGL, this);
#endif

    Graphics::catch_error();
    return *this;
}

Graphics& Graphics::clear(Color color) {

    glClearColor(
        color[0],
        color[1],
        color[2],
        color[3]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return *this;
}

Graphics& Graphics::viewport(IVec2 size) {
    glViewport(0, 0, size[0], size[1]);
    this->viewport_ = size;
    return *this;
}

void Graphics::catch_error() {

    GLenum error = glGetError();

    std::unordered_map<unsigned int, std::string> errors {
        {GL_INVALID_ENUM, "GL_INVALID_ENUM"},
        {GL_INVALID_VALUE, "GL_INVALID_VALUE"},
        {GL_INVALID_OPERATION, "GL_INVALID_OPERATION"},
        {GL_OUT_OF_MEMORY, "GL_OUT_OF_MEMORY"},
        {GL_INVALID_FRAMEBUFFER_OPERATION, "GL_INVALID_FRAMEBUFFER_OPERATION"},
    };

    std::stringstream ss;

    while (error != GL_NO_ERROR) {

        try {
            ss << "GL error: " << errors.at(error) << "\n";
        } catch (...) {
            throw std::runtime_error("Unknown GL error");
        }

        error = glGetError();
    }

    if (ss.rdbuf()->in_avail() != 0) {
        throw std::runtime_error(ss.str());
    }
}

} // end of namespace wuh