#include<iostream>

#include <GLFW/glfw3.h>
#include <VkBootstrap.h>
#include <VkBootstrapDispatch.h>

#define SUCCESS 1
#define FAIL 0

typedef struct {
    GLFWwindow* window;
    vkb::InstanceBuilder builder;
    vkb::Instance vkb_inst;
    vkb::PhysicalDeviceSelector selector;
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
    int status = SUCCESS;
    auto inst_ret = vkCtl.builder.set_app_name ("Example Vulkan Application")
                        .request_validation_layers ()
                        .use_default_debug_messenger ()
                        .build ();

    if (!inst_ret) {
        status = FAIL;
        std::cout << "Failed to create Vulkan instance. Error: " << inst_ret.error().message() << "\n";
        return status;
    } 
    else {
        vkCtl.vkb_inst = inst_ret.value ();
    }

 








    // if (!phys_ret) {
    //     status = FAIL;
    //     std::cerr << "Failed to select Vulkan Physical Device. Error: " << phys_ret.error().message() << "\n";
    //     return status;
    // }
    // else {

    // }

    return status;
}

int main() {

    if (initWindow()) { std::cout << "success::initWindow" << std::endl;}
    if (initVulkan()) { std::cout << "success::initVulkan " << std::endl;}

    return 0;
}