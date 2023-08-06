#include<iostream>

#include <GLFW/glfw3.h>
#include <VkBootstrap.h>
#include <VkBootstrapDispatch.h>

#define SUCCESS 1
#define FAIL 0

typedef struct {
    vkb::InstanceBuilder instance_builder;
} vk_s;

vk_s vkCtl;

int initWindow (GLFWwindow* window) {
    int status = SUCCESS;
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    if ((window = glfwCreateWindow(1024, 1024, "test", NULL, NULL)) == NULL) {
        std::cout << "window creation FAILED!" << std::endl;
        status = FAIL;
    }

    return status;
}

int main() {

    GLFWwindow* window = NULL;

    if (initWindow(window)) { std::cout << "success::window created" << std::endl;}

    return 0;
}