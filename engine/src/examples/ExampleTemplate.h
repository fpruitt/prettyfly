// Example template for creating new examples
// Copy this file and implement your custom example

#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ExampleBase.h"
#include "../core/Input.h"
#include "../rendering/RenderContext.h"
#include "../rendering/Shader.h"
#include "../rendering/ShaderSources.h"

class MyCustomExample : public ExampleBase
{
public:
    void run(GLFWwindow* window) override
    {
        // 1. Create shaders
        Shader shader(ShaderSources::basicVertex, ShaderSources::basicFragment);

        // 2. Create geometry (VAO/VBO setup)
        // ... your setup code here ...

        // 3. Main loop
        while (!glfwWindowShouldClose(window))
        {
            // Handle input
            if (Input::isKeyPressed(KeyCode::Escape))
            {
                glfwSetWindowShouldClose(window, true);
            }

            // Clear screen
            RenderContext::setClearColor(0.1f, 0.1f, 0.15f);
            RenderContext::clear();

            // Draw
            shader.use();
            // ... your drawing code here ...

            // Swap and poll
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        // 4. Cleanup
        // ... delete VAOs/VBOs ...
    }
};

/*
To use this example:
1. Add to main.cpp:
   #include "examples/MyCustomExample.h"

2. In main(), create the example:
   example = std::make_unique<MyCustomExample>();
*/
