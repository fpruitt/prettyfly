#pragma once

namespace ShaderSources {

const char* basicVertex = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    uniform mat4 MVP;
    void main() {
        gl_Position = MVP * vec4(aPos, 1.0);
    }
)";

const char* basicFragment = R"(
    #version 330 core
    out vec4 FragColor;
    void main() {
        FragColor = vec4(0.6, 0.7, 0.9, 1.0);
    }
)";

const char* coloredVertex = R"(
    #version 330 core
    layout (location = 0) in vec3 pos;
    layout (location = 1) in vec3 color;
    out vec3 vColor;
    void main() {
        gl_Position = vec4(pos, 1.0);
        vColor = color;
    }
)";

const char* coloredFragment = R"(
    #version 330 core
    in vec3 vColor;
    out vec4 FragColor;
    void main() {
        FragColor = vec4(vColor, 1.0);
    }
)";

const char* litVertex = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;

    uniform mat4 MVP;
    uniform mat4 Model;

    out vec3 fragNormal;
    out vec3 fragWorldPos;

    void main() {
        gl_Position  = MVP * vec4(aPos, 1.0);
        fragWorldPos = vec3(Model * vec4(aPos, 1.0));
        fragNormal = mat3(transpose(inverse(Model))) * aNormal;
    }
)";

const char* litFragment = R"(
    #version 330 core
    in vec3 fragNormal;
    in vec3 fragWorldPos;

    uniform vec3 lightDir;
    uniform vec3 lightColor;
    uniform vec3 objectColor;

    out vec4 FragColor;

    void main() {
        vec3 norm = normalize(fragNormal);
        if (!gl_FrontFacing) norm = -norm;

        float ambientStrength = 0.2;
        vec3 ambient = ambientStrength * lightColor;

        float diff   = max(dot(norm, normalize(lightDir)), 0.0);
        vec3 diffuse = diff * lightColor;

        vec3 result = (ambient + diffuse) * objectColor;
        FragColor   = vec4(result, 1.0);
    }
)";

} // namespace ShaderSources
