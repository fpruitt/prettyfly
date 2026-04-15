#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

class RenderContext
{
public:
    static bool initialize(GLFWwindow* window)
    {
        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cerr << "Failed to initialize GLAD\n";
            return false;
        }

        // Flush error queue
        while (glGetError() != GL_NO_ERROR) {}

        // Default OpenGL state
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        return true;
    }

    static void setClearColor(float r, float g, float b, float a = 1.0f)
    {
        glClearColor(r, g, b, a);
    }

    static void clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    static void checkError(const char* label)
    {
        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
        {
            std::cerr << "GL error at [" << label << "]: " << err << "\n";
        }
    }
};
