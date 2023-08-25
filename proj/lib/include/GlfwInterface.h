#pragma once
#include<iostream>
#include <GLFW/glfw3.h>
#include "globalDefs.h"

#define GLFW_INCLUDE_VULKAN

class GlfwInterface {
    public:
        GlfwInterface();
        ~GlfwInterface();

        int initWindow();

        GLFWwindow* window;
    private:
};