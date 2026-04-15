#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ExampleBase.h"
#include "../rendering/Shader.h"
#include "../rendering/ShaderSources.h"
#include "../rendering/RenderContext.h"
#include "../core/Input.h"

class DebugShapes : public ExampleBase
{
public:
    void run(GLFWwindow* window) override
    {
        Shader shader(ShaderSources::coloredVertex, ShaderSources::coloredFragment);
        
        float airplaneVerts[] = {
            // Left wing
            0.0f,  0.0f, 0.0f,   0.5f, 0.6f, 0.9f,
            -0.5f, -0.1f, 0.0f,   0.5f, 0.6f, 0.9f,
            -0.05f, -0.15f, 0.0f, 0.5f, 0.6f, 0.9f,

            // Right wing
            0.0f,  0.0f, 0.0f,   0.5f, 0.6f, 0.9f,
            0.05f, -0.15f, 0.0f, 0.5f, 0.6f, 0.9f,
            0.5f, -0.1f, 0.0f,   0.5f, 0.6f, 0.9f,

            // Left tail
            0.0f, -0.3f, 0.0f,   0.6f, 0.5f, 0.9f,
            -0.2f, -0.4f, 0.0f,   0.6f, 0.5f, 0.9f,
            -0.03f, -0.4f, 0.0f,  0.6f, 0.5f, 0.9f,

            // Right tail
            0.0f, -0.3f, 0.0f,   0.6f, 0.5f, 0.9f,
            0.03f, -0.4f, 0.0f,  0.6f, 0.5f, 0.9f,
            0.2f, -0.4f, 0.0f,   0.6f, 0.5f, 0.9f,

            // Fuselage (thin vertical rectangle)
            0.0f,  0.4f, 0.0f,   0.8f, 0.8f, 0.8f,
            -0.03f, -0.4f, 0.0f,  0.8f, 0.8f, 0.8f,
            0.03f, -0.4f, 0.0f,  0.8f, 0.8f, 0.8f,

            0.0f,  0.4f, 0.0f,   0.8f, 0.8f, 0.8f,
            0.03f, -0.4f, 0.0f,  0.8f, 0.8f, 0.8f,
            0.00f,  0.4f, 0.0f,  0.8f, 0.8f, 0.8f,
        };

        GLuint VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(airplaneVerts), airplaneVerts, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glDisable(GL_DEPTH_TEST);
        glFlush();

        while (!glfwWindowShouldClose(window))
        {
            if (Input::isKeyPressed(KeyCode::Escape))
            {
                glfwSetWindowShouldClose(window, true);
            }

            RenderContext::setClearColor(0.1f, 0.1f, 0.15f);
            RenderContext::clear();

            shader.use();
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 18);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
};
