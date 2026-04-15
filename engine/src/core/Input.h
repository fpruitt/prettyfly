#pragma once
#include <GLFW/glfw3.h>
#include <functional>
#include <unordered_map>

enum class KeyCode {
    W = GLFW_KEY_W,
    A = GLFW_KEY_A,
    S = GLFW_KEY_S,
    D = GLFW_KEY_D,
    Space = GLFW_KEY_SPACE,
    LeftControl = GLFW_KEY_LEFT_CONTROL,
    Escape = GLFW_KEY_ESCAPE
};

class Input
{
public:
    static void initialize(GLFWwindow* window)
    {
        s_window = window;

        glfwSetCursorPosCallback(window, [](GLFWwindow*, double xpos, double ypos)
        {
            if (s_mouseMoveCallback)
            {
                s_mouseMoveCallback((float)xpos, (float)ypos);
            }
        });
    }

    static bool isKeyPressed(KeyCode key)
    {
        return glfwGetKey(s_window, static_cast<int>(key)) == GLFW_PRESS;
    }

    static void setMouseMoveCallback(std::function<void(float, float)> callback)
    {
        s_mouseMoveCallback = callback;
    }

    static void captureMouse(bool capture)
    {
        glfwSetInputMode(s_window, GLFW_CURSOR, 
                        capture ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }

private:
    static inline GLFWwindow* s_window = nullptr;
    static inline std::function<void(float, float)> s_mouseMoveCallback;
};
