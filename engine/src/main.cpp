#define CGLTF_IMPLEMENTATION
#include "vendor/cgltf.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "core/Config.h"
#include "core/Input.h"
#include "rendering/RenderContext.h"
#include "examples/DebugShapes.h"
#include "examples/MeshViewer.h"

#include <iostream>
#include <filesystem>
#include <memory>

// TODO this should be dynamic based on screen size / config
const int DEFAULT_WINDOW_SIZE_X = 1920;
const int DEFAULT_WINDOW_SIZE_Y = 1080;

const int GLFW_CONTEXT_VERSION_MAJOR_VAL = 3;
const int GLFW_CONTEXT_VERSION_MINOR_VAL = 3;

int main()
{
    std::cout << "Working directory: " << std::filesystem::current_path() << "\n";

    // TODO fix this path nonsense, we should be specifying paths from the project root and not relying on relative
    // paths from the executable. (this is the first pass of the config system)
    Config::instance().load("../../../config/engine.ini");

    int width = Config::instance().getInt("Window", "width");
    int height = Config::instance().getInt("Window", "height");

#ifdef PF_DEBUG
    const char* windowTitle = "PrettyFly (Debug)";
#else
    const char* windowTitle = "PrettyFly";
#endif

    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GLFW_CONTEXT_VERSION_MAJOR_VAL);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GLFW_CONTEXT_VERSION_MINOR_VAL);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = nullptr;
    GLFWwindow* sharedWindow = nullptr;

    GLFWwindow* window = glfwCreateWindow(DEFAULT_WINDOW_SIZE_X, DEFAULT_WINDOW_SIZE_Y, windowTitle, monitor, sharedWindow);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    if (!RenderContext::initialize(window))
    {
        glfwTerminate();
        return -1;
    }

    Input::initialize(window);

    // Select which example to run
    std::unique_ptr<ExampleBase> example;

    // Option 1: Debug airplane shape
    //example = std::make_unique<DebugShapes>();

    // Option 2: Mesh viewer with lighting and camera controls
    example = std::make_unique<MeshViewer>("../../../assets/models/f22_raptor/scene.gltf");

    if (example)
    {
        example->run(window);
    }

    glfwTerminate();
    return 0;
}

