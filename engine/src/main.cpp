#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

const char* vertSrc = R"(
    #version 330 core
    layout (location = 0) in vec3 pos;
    layout (location = 1) in vec3 color;
    out vec3 vColor;
    void main() {
        gl_Position = vec4(pos, 1.0);
        vColor = color;
    }
)";

const char* fragSrc = R"(
    #version 330 core
    in vec3 vColor;
    out vec4 FragColor;
    void main() {
        FragColor = vec4(vColor, 1.0);
    }
)";

const char* engineName = "PrettyFly";

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
}

void drawDebugAirplane(GLFWwindow* window, const GLuint& shader) {
    float verts[] = {

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
        -0.03f,  0.4f, 0.0f,  0.8f, 0.8f, 0.8f, 
    };

    drawDebugShape(window, shader, verts, 24);
}

void drawDebugTriangle(GLFWwindow* window, const GLuint& shader) {
    // Triangle: interleaved position + color
    float verts[] = {
        // pos              // color
         0.0f,  0.5f, 0.0f,  1.0f, 0.2f, 0.2f,
        -0.5f, -0.5f, 0.0f,  0.2f, 1.0f, 0.2f,
         0.5f, -0.5f, 0.0f,  0.2f, 0.2f, 1.0f,
    };

    drawDebugShape(window, shader, verts, 3);
}

int main() {

    
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

    GLFWwindow* window = glfwCreateWindow(1280, 720, engineName, nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // Compile shaders
    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vertSrc, nullptr);
    glCompileShader(vert);

    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fragSrc, nullptr);
    glCompileShader(frag);

    GLuint shader = glCreateProgram();
    glAttachShader(shader, vert);
    glAttachShader(shader, frag);
    glLinkProgram(shader);
    glDeleteShader(vert);
    glDeleteShader(frag);

    drawDebugAirplane(window, shader);

    glfwTerminate();
    return 0;
}
