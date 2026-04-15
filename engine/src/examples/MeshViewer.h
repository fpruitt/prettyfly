#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ExampleBase.h"
#include "../core/Camera.h"
#include "../core/MeshLoader.h"
#include "../core/Input.h"
#include "../rendering/Shader.h"
#include "../rendering/ShaderSources.h"
#include "../rendering/RenderContext.h"
#include <iostream>

class MeshViewer : public ExampleBase
{
public:
    MeshViewer(const char* meshPath) : m_meshPath(meshPath) {}

    void run(GLFWwindow* window) override
    {
        Shader shader(ShaderSources::litVertex, ShaderSources::litFragment);
        
        while (glGetError() != GL_NO_ERROR) {}
        Mesh mesh = loadGLTF(m_meshPath);
        RenderContext::checkError("load mesh");

        Camera camera;
        camera.position = glm::vec3(0.0f, 2.0f, 20.0f);
        camera.updateVectors();

        Input::captureMouse(true);
        Input::setMouseMoveCallback([&camera](float x, float y)
        {
            camera.processMouse(x, y);
        });

        glm::mat4 model = glm::mat4(1.0f);
        glEnable(GL_DEPTH_TEST);

        float lastFrame = 0.0f;
        std::cout << "Entering mesh viewer loop...\n";

        while (!glfwWindowShouldClose(window))
        {
            float currentFrame = (float)glfwGetTime();
            float deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            camera.processInput(deltaTime);

            if (Input::isKeyPressed(KeyCode::Escape))
            {
                glfwSetWindowShouldClose(window, true);
            }

            RenderContext::setClearColor(0.1f, 0.1f, 0.15f);
            RenderContext::clear();

            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            float aspect = (float)width / (float)height;

            glm::mat4 view = camera.getViewMatrix();
            glm::mat4 proj = camera.getProjectionMatrix(aspect);
            glm::mat4 MVP = proj * view * model;

            shader.use();
            shader.setMat4("MVP", MVP);
            shader.setMat4("Model", model);
            shader.setVec3("lightDir", glm::normalize(glm::vec3(1.0f, 2.0f, 1.0f)));
            shader.setVec3("lightColor", 1.0f, 1.0f, 0.95f);
            shader.setVec3("objectColor", 0.7f, 0.75f, 0.8f);

            glBindVertexArray(mesh.VAO);
            RenderContext::checkError("bindVAO");

            if (mesh.indexed)
            {
                glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
            }
            else
            {
                glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount);
            }

            RenderContext::checkError("draw");

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        mesh.free();
    }

private:
    const char* m_meshPath;
};
