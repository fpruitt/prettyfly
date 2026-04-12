#define CGLTF_IMPLEMENTATION
#include "vendor/cgltf.h"

#include "core/MeshLoader.h"
#include "core/Camera.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <filesystem>


const char* vertexShaderWithColorSrc = R"(
    #version 330 core
    layout (location = 0) in vec3 pos;
    layout (location = 1) in vec3 color;
    out vec3 vColor;
    void main() {
        gl_Position = vec4(pos, 1.0);
        vColor = color;
    }
)";

const char* vertexShaderPositionOnlySrc = R"(
    #version 330 core
    layout (location = 0) in vec3 pos;
    void main() {
        gl_Position = vec4(pos, 1.0);
    }
)";

const char* vertexShaderMVP = R"(
    #version 330 core
    layout (location = 0) in vec3 pos;
    uniform mat4 MVP;
    void main() {
        gl_Position = MVP * vec4(pos, 1.0);
    }
)";

const char* fragmentShaderWithColorSrc = R"(
    #version 330 core
    in vec3 vColor;
    out vec4 FragColor;
    void main() {
        FragColor = vec4(vColor, 1.0);
    }
)";

const char* fragmentShaderNoColorSrc = R"(
    #version 330 core
    out vec4 FragColor;
    void main() {
        FragColor = vec4(0.6, 0.7, 0.9, 1.0);
    }
)";

const char* engineName = "PrettyFly";


auto checkGL = [](const char* label)
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cerr << "GL error at [" << label << "]: " << err << "\n";
    };
};

auto checkShader = [](GLuint shader, const char* label) {
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader compile error [" << label << "]: " << log << "\n";
    }
};

void drawDebugShape(GLFWwindow* window, const GLuint& shader, const float verts[], int vertexCount) {
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * 6 * sizeof(float), verts, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glDisable(GL_DEPTH_TEST);

    // Ensure buffer upload completes before rendering
    glFlush();

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void drawMesh(GLFWwindow* window, const GLuint& shader, const Mesh& mesh)
{
    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader);
        glBindVertexArray(mesh.VAO);
        if (mesh.indexed)
        {
            glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
        }
        else
        {
            glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount);
        }

        checkGL("draw");

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void drawDebugAirplane(GLFWwindow* window, const GLuint& shader)
{
    float verts[] =
    {

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

    drawDebugShape(window, shader, verts, 18);
}

void drawDebugTriangle(GLFWwindow* window, const GLuint& shader)
{
    // Triangle: interleaved position + color
    float verts[] = {
        // pos              // color
         0.0f,  0.5f, 0.0f,  1.0f, 0.2f, 0.2f,
        -0.5f, -0.5f, 0.0f,  0.2f, 1.0f, 0.2f,
         0.5f, -0.5f, 0.0f,  0.2f, 0.2f, 1.0f,
    };

    drawDebugShape(window, shader, verts, 3);
}

// Todo move this to a separate file and add more examples
void exampleDebugDrawMain(GLFWwindow* window, const char* vertexShaderSrc, const char* fragmentShaderSrc)
{
    // Compile shaders
    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vertexShaderSrc, nullptr);
    glCompileShader(vert);

    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fragmentShaderSrc, nullptr);
    glCompileShader(frag);

    GLuint shader = glCreateProgram();
    glAttachShader(shader, vert);
    glAttachShader(shader, frag);
    glLinkProgram(shader);
    glDeleteShader(vert);
    glDeleteShader(frag);

    drawDebugAirplane(window, shader);

    glfwTerminate();
}

// Todo move this to a separate file and add more examples
void exampleDebugDrawMeshWithPerspective(GLFWwindow* window, const char* vertexShaderSrc, const char* fragmentShaderSrc, const char* meshSrc)
{
    // Compile shaders
    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vertexShaderSrc, nullptr);
    glCompileShader(vert);
    checkGL("compile vert");

    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fragmentShaderSrc, nullptr);
    glCompileShader(frag);
    checkGL("compile frag");

    GLuint shader = glCreateProgram();
    glAttachShader(shader, vert);
    glAttachShader(shader, frag);
    glLinkProgram(shader);
    checkGL("link shader");

    // Check link status
    checkShader(vert, "vertex shader");
    checkShader(frag, "fragment shader");


    glDeleteShader(vert);
    glDeleteShader(frag);

    // Cache uniform location
    GLint mvpLocation = glGetUniformLocation(shader, "MVP");
    checkGL("getUniformLocation");

    // Load the mesh from the GLTF file
    while (glGetError() != GL_NO_ERROR) {} // flush before mesh load
    Mesh mesh = loadGLTF(meshSrc);
    checkGL("load");

    // Set Up The Camera
    Camera camera;
    camera.position = glm::vec3(0.0f, 2.0f, 20.0f);
    camera.updateVectors();

    // Capture mouse cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // Mouse callback — GLFW needs a free function or lambda, so store camera as static for now
    // TODO: refactor this when we have a proper app class, this is hacky AF
    static Camera* activeCam = &camera;
    glfwSetCursorPosCallback(window, [](GLFWwindow*, double xpos, double ypos) {
        activeCam->processMouse((float)xpos, (float)ypos);
    });

    //glm::vec3 cameraPosition = glm::vec3(0.0f, 5.0f, 20.0f);
    //glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    //glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    // TODO- Projection will change on window resize, need to handle that event
    glm::mat4 projection = glm::perspective(
        glm::radians(60.f),
        1920.f / 1080.f, // TODO- get actual window size
        0.1f,
        1000.f
    );

    // Identity model matrix so the drawn mesh sits at 0,0,0 with no rotation or scale applied for now.
    glm::mat4 model = glm::mat4(1.0f);

    // OpenGL depth test
    glEnable(GL_DEPTH_TEST);

    // Main Loop
    while (glGetError() != GL_NO_ERROR) {} // flush before loop
    std::cout << "Entering game loop...\n";

    static float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {

        // Delta time
        // TODO this should be input into main loop, just getting started with a single example function for now
        
        float currentFrame = (float)glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input
        camera.processKeyboard(window, deltaTime);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Build MVP from camera
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float aspect = (float)width / (float)height;

        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 proj = camera.getProjectionMatrix(aspect);
        glm::mat4 MVP = proj * view * model;

        //glm::mat4 view = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
        //glm::mat4 MVP = projection * view * model;

        // Draw the mesh
        glUseProgram(shader);
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(MVP));

        //std::cerr << "mesh.VAO: " << mesh.VAO << "\n";
        glBindVertexArray(mesh.VAO);
        checkGL("bindVAO");

        // Check what's actually bound in this VAO
        GLint eboBinding;
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &eboBinding);
        //std::cerr << "EBO bound: " << eboBinding << "\n";

        GLint vboBinding;
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vboBinding);
        //std::cerr << "VBO bound: " << vboBinding << "\n";

        //std::cerr << "mesh.indexed: " << mesh.indexed << "\n";
        //std::cerr << "mesh.indexCount: " << mesh.indexCount << "\n";
        //std::cerr << "mesh.EBO: " << mesh.EBO << "\n";

        if (mesh.indexed)
        {
            glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
        }
        else
        {
            glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount);
        }

        checkGL("draw");

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    mesh.free();
    glDeleteProgram(shader);
    glfwTerminate();
}

// Todo move this to a separate file and add more examples
void exampleDebugDrawMesh(GLFWwindow* window, const char* vertexShaderSrc, const char* fragmentShaderSrc, const char* meshSrc)
{
    // Compile shaders
    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vertexShaderSrc, nullptr);
    glCompileShader(vert);

    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fragmentShaderSrc, nullptr);
    glCompileShader(frag);

    GLuint shader = glCreateProgram();
    glAttachShader(shader, vert);
    glAttachShader(shader, frag);
    glLinkProgram(shader);
    glDeleteShader(vert);
    glDeleteShader(frag);

    Mesh mesh = loadGLTF(meshSrc);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "OpenGL error after mesh load: " << err << "\n";
    }
    drawMesh(window, shader, mesh);
    mesh.free();
    glfwTerminate();
}

int main()
{
    std::cout << "Working directory: " << std::filesystem::current_path() << "\n";
    
    // TODO Handle more than just debug/release
#ifdef PF_DEBUG
    const char* windowTitle = "PrettyFly (Debug)";
#else
    const char* windowTitle = "PrettyFly";
#endif

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1920, 1080, windowTitle, nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    while (glGetError() != GL_NO_ERROR) {} // flush error queue
    checkGL("context init");

    //exampleDebugDrawMain(window, vertexShaderWithColorSrc, fragmentShaderWithColorSrc);
    //exampleDebugDrawMesh(window, vertexShaderPositionOnlySrc, fragmentShaderNoColorSrc, "../../../assets/models/f22_raptor/scene.gltf");
    exampleDebugDrawMeshWithPerspective(window, vertexShaderMVP, fragmentShaderNoColorSrc, "../../../assets/models/f22_raptor/scene.gltf");

    return 0;
}
