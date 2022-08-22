#include "Main.hpp"

Main *Main::instance = nullptr;

Main::Main(int argc, char *argv[]) : data{argc, argv}
{
    Main::instance = this;
    this->initGraphics();
    this->load();
    this->resume();
}

void Main::_glfwSizeCallback(GLFWwindow *window, int width, int height)
{
    Main::instance->screen_width = width;
    Main::instance->screen_height = height;

    glViewport(0, 0, width, height);
}

void Main::_glfwCursorCallback(GLFWwindow *window, double xpos, double ypos)
{
    Main::instance->engine.cursor[0][0] = xpos;
    Main::instance->engine.cursor[0][1] = ypos;
}

void Main::_glfwInputCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS)
        return;

    switch (key)
    {
    case GLFW_KEY_ESCAPE:

        if (!Main::instance->mainScene->onEventBack())
        {
            exit(0);
        }

        break;
    case GLFW_KEY_LEFT:
        Main::instance->mainScene->onEventLeft();
        break;
    case GLFW_KEY_UP:
        Main::instance->mainScene->onEventUp();
        break;
    case GLFW_KEY_RIGHT:
        Main::instance->mainScene->onEventRight();
        break;
    case GLFW_KEY_DOWN:
        Main::instance->mainScene->onEventDown();
        break;
    case GLFW_KEY_ENTER:
        Main::instance->mainScene->onEventSelect();
        break;
    default:
        break;
    }
}

void Main::_glfwMouseCallback(GLFWwindow *window, int button, int action, int mods)
{
    if (action != GLFW_PRESS)
        return;

    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
        Main::instance->mainScene->onEventPointerUp();
        break;
    }
}

static void GLAPIENTRY errorOccurredGL(
    GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar *message, const void *userParam)
{
    (void)length;
    (void)userParam;

    const char* _source;
    const char* _type;
    const char* _severity;

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
        _source = "UNKNOWN";
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

    printf("Message from OpenGL:\nSource: %s\nType: %s\n"
           "Id: 0x%x\nSeverity: %s\n",
           _source, _type, id, _severity);

    printf("%s\n", message);

    if (type == GL_DEBUG_SEVERITY_HIGH)
        exit(-1);
}

void Main::load()
{
    if (!mainScene)
        mainScene = std::make_unique<MainScene>();
}

void Main::unload()
{
    mainScene->pause();
}

void Main::run()
{
    this->mainScene->resume();

    float deltaTime = 0.0f; // TODO
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(this->window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        this->render();
    }
}

void Main::render()
{
    /* Poll for and process events */
    glfwPollEvents();

    glClearColor(
        this->mainScene->backgroundColor[0],
        this->mainScene->backgroundColor[1],
        this->mainScene->backgroundColor[2],
        this->mainScene->backgroundColor[3]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Rendering scene */
    if (!this->mainScene->draw())
    {
        glfwSetWindowShouldClose(this->window, 1);
    }

    /* Swap front and back buffers */
    glfwSwapBuffers(this->window);
}

void Main::initGraphics()
{
    /* Initialize the library */
    if (!glfwInit())
        exit(-1);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

#if defined(__linux__)

    if (glfwPlatformSupported(GLFW_PLATFORM_WAYLAND))
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
    else if (glfwPlatformSupported(GLFW_PLATFORM_X11))
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#endif

    /* Create a windowed mode window and its OpenGL context */
    this->window = glfwCreateWindow(this->screen_width, this->screen_height, "LinHop", NULL, NULL);

    if (!this->window)
    {
        glfwTerminate();
        exit(-1);
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(this->window);
    glfwSetFramebufferSizeCallback(this->window, Main::_glfwSizeCallback);
    glfwSwapInterval(1);
    glfwSetCursorPosCallback(this->window, Main::_glfwCursorCallback);
    glfwSetKeyCallback(this->window, Main::_glfwInputCallback);
    glfwSetMouseButtonCallback(this->window, Main::_glfwMouseCallback);

    if (glewInit() != GLEW_OK)
        exit(-1);

    /* Set view parameters */
    glClearColor(0.0f, 0.1f, 0.2f, 1.0f);
    glViewport(0, 0, this->screen_width, this->screen_height);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);

    /* Set texturing parameters */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* Set debug mode */
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(errorOccurredGL, NULL);

    spige_init(&this->engine);
    spige_viewport(&engine, this->screen_width, this->screen_height);

    LOGI("GL Init: %d", GL_VERSION);
}

void Main::terminateGraphics()
{
    spige_destroy(&this->engine);
}

/**
 * This is the main entry point
 */
#if defined(WIN32) || defined(_WIN32)
#include <Windows.h>

int APIENTRY WinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR lpCmdLine,
                     _In_ int nCmdShow)
{

    (void)hInstance, (void)hPrevInstance, (void)lpCmdLine, (void)nCmdShow;

    Main main(__argc, __argv);
    main.run();
}
#else
int main(int argc, char *argv[])
{
    Main main(argc, argv);
    main.run();
}
#endif
