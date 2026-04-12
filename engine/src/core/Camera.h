#pragma once
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <cmath>


struct Camera {
    glm::vec3 position = glm::vec3(0.0f, 2.0f, 20.0f);
    float yaw = -90.0f; // facing -Z by default
    float pitch = 0.0f;
    float moveSpeed = 10.0f;
    float mouseSensivity = 0.1f;
    float fov = 60.0f;

    // Derived each frame from yaw/pitch
    glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    bool firstMouse = true;
    float lastMouseX = 0.0f;
    float lastMouseY = 0.0f;

    void updateVectors() {
        glm::vec3 f;
        f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        f.y = sin(glm::radians(pitch));
        f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        forward = glm::normalize(f);
        right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
        up = glm::normalize(glm::cross(right, forward));
    }

    void processKeyboard(GLFWwindow* window, float deltaTime) {
        float velocity = moveSpeed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) position += forward * velocity;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) position -= forward * velocity;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) position -= right * velocity;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) position += right * velocity;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) position += glm::vec3(0, 1, 0) * velocity;
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) position -= glm::vec3(0, 1, 0) * velocity;
    }

    void processMouse(float xpos, float ypos) {
        if (firstMouse) {
            lastMouseX = xpos;
            lastMouseY = ypos;
            firstMouse = false;
        }

        float xoffset = (xpos - lastMouseX) * mouseSensivity;
        float yoffset = (lastMouseY - ypos) * mouseSensivity; // inverted Y
        lastMouseX = xpos;
        lastMouseY = ypos;

        yaw += xoffset;
        pitch += yoffset;
        pitch = glm::clamp(pitch, -89.0f, 89.0f); // prevent gimbal flip

        updateVectors();
    }

    glm::mat4 getViewMatrix() const {
        return glm::lookAt(position, position + forward, up);
    }

    glm::mat4 getProjectionMatrix(float aspectRatio) const {
        return glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 10000.0f);
    }
};
