#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <iostream>

class Shader {
public:
    GLuint program = 0;

    Shader() = default;

    Shader(const char* vertexSource, const char* fragmentSource)
    {
        compile(vertexSource, fragmentSource);
    }

    ~Shader()
    {
        if (program)
        {
            glDeleteProgram(program);
        }
    }

    // Prevent copying
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // Allow moving
    Shader(Shader&& other) noexcept : program(other.program)
    {
        other.program = 0;
    }

    Shader& operator=(Shader&& other) noexcept
    {
        if (this != &other)
        {
            if (program)
            {
                glDeleteProgram(program);
            }
            program = other.program;
            other.program = 0;
        }
        return *this;
    }

    bool compile(const char* vertexSource, const char* fragmentSource)
    {
        GLuint vert = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vert, 1, &vertexSource, nullptr);
        glCompileShader(vert);

        if (!checkShaderCompilation(vert, "Vertex"))
        {
            glDeleteShader(vert);
            return false;
        }

        GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(frag, 1, &fragmentSource, nullptr);
        glCompileShader(frag);

        if (!checkShaderCompilation(frag, "Fragment"))
        {
            glDeleteShader(vert);
            glDeleteShader(frag);
            return false;
        }

        program = glCreateProgram();
        glAttachShader(program, vert);
        glAttachShader(program, frag);
        glLinkProgram(program);

        glDeleteShader(vert);
        glDeleteShader(frag);

        GLint success;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success)
        {
            char log[512];
            glGetProgramInfoLog(program, 512, nullptr, log);
            std::cerr << "Shader linking error: " << log << "\n";
            return false;
        }

        return true;
    }

    void use() const
    {
        glUseProgram(program);
    }

    void setMat4(const char* name, const glm::mat4& mat) const
    {
        GLint loc = glGetUniformLocation(program, name);
        if (loc != -1)
        {
            glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
        }
    }

    void setVec3(const char* name, const glm::vec3& vec) const
    {
        GLint loc = glGetUniformLocation(program, name);
        if (loc != -1)
        {
            glUniform3fv(loc, 1, glm::value_ptr(vec));
        }
    }

    void setVec3(const char* name, float x, float y, float z) const
    {
        GLint loc = glGetUniformLocation(program, name);
        if (loc != -1)
        {
            glUniform3f(loc, x, y, z);
        }
    }

    void setFloat(const char* name, float value) const
    {
        GLint loc = glGetUniformLocation(program, name);
        if (loc != -1)
        {
            glUniform1f(loc, value);
        }
    }

private:
    bool checkShaderCompilation(GLuint shader, const char* type) const
    {
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char log[512];
            glGetShaderInfoLog(shader, 512, nullptr, log);
            std::cerr << type << " shader compilation error: " << log << "\n";
            return false;
        }
        return true;
    }
};
