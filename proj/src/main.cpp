#include<iostream>

#include <GLFW/glfw3.h>
#include <VkBootstrap.h>
#include <VkBootstrapDispatch.h>

#define SUCCESS 1
#define FAIL 0

typedef struct {
    GLFWwindow* window;
    vkb::Instance vkb_inst;
    VkSurfaceKHR surface;
    vkb::Device device;
    vkb::Swapchain swapchain;
} vk_s;

vk_s vkCtl = {};

int initWindow () {
    int status = SUCCESS;
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    if ((vkCtl.window = glfwCreateWindow(1024, 1024, "test", NULL, NULL)) == NULL) {
        std::cout << "window creation FAILED!" << std::endl;
        status = FAIL;
    }

    return status;
}

int initVulkan() {

    return status;
}

int main() {

    if (initWindow()) { std::cout << "success::initWindow" << std::endl;}
    if (initVulkan()) { std::cout << "success::initVulkan " << std::endl;}

    return 0;
}