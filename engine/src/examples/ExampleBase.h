#pragma once

struct GLFWwindow;

class ExampleBase {
public:
    virtual ~ExampleBase() = default;

    virtual void run(GLFWwindow* window) = 0;
};
