#pragma once
#include<iostream>
#include <GLFW/glfw3.h>
#include "globalDefs.h"

#define GLFW_INCLUDE_VULKAN

typedef struct glfwInput {
    double cursorX;
    double cursorY;
    bool rightMb;
    bool leftMb;
    bool midMb;
} glfwInput_t;

class GlfwInterface {
    public:
        GlfwInterface();
        ~GlfwInterface();
        int initWindow();

        static glfwInput_t winInput;
        static void cursorPosition(GLFWwindow* win, double x, double y);
        static void mouseButtonPress(GLFWwindow* window, int button, int action, int mods);
        static void buttonHandle(int action, bool *button);

        GLFWwindow* window;

    private:
};