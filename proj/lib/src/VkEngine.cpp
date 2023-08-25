#include "VkEngine.h"

VkEngine::VkEngine() {
    if (initGlfw()) { std::cout << "VkEngine::initGlfw SUCCESS!" << std::endl; }
    if (initVulkan()) { std::cout << "VkEngine::initVulkan SUCCESS!" << std::endl; }
}

int VkEngine::initGlfw() { 
    int status = SUCCESS;
    vkCtl.glfw = new GlfwInterface(); 
    if (vkCtl.glfw->window == NULL) { status = FAIL; }
    return status;
}

int VkEngine::initVulkan() {
    vkb::InstanceBuilder builder;
    auto inst_ret = builder.set_app_name ("Example Vulkan Application")
                        .request_validation_layers ()
                        .use_default_debug_messenger ()
                        .build ();
    if (!inst_ret) {
        std::cerr << "Failed to create Vulkan instance. Error: " << inst_ret.error().message() << "\n";
        return FAIL;
    }
    // initialized vulkan instance
    vkCtl.vkbInst = inst_ret.value ();
    
    glfwCreateWindowSurface(vkCtl.vkbInst.instance, vkCtl.glfw->window, NULL, &vkCtl.surface);
    vkb::PhysicalDeviceSelector selector{  vkCtl.vkbInst };
    auto phys_ret = selector.set_surface (vkCtl.surface)
                        .set_minimum_version (1, 1) // require a vulkan 1.1 capable device
                        .require_dedicated_transfer_queue ()
                        .select ();
    if (!phys_ret) {
        std::cerr << "Failed to select Vulkan Physical Device. Error: " << phys_ret.error().message() << "\n";
        return FAIL;
    }

    vkb::DeviceBuilder device_builder{ phys_ret.value () };
    auto dev_ret = device_builder.build ();
    if (!dev_ret) {
        std::cerr << "Failed to create Vulkan device. Error: " << dev_ret.error().message() << "\n";
        return FAIL;
    }
    // initialize vulkan device
    vkCtl.vkbDevice = dev_ret.value ();

    // Get the graphics queue with a helper function
    auto graphics_queue_ret =  vkCtl.vkbDevice.get_queue (vkb::QueueType::graphics);
    if (!graphics_queue_ret) {
        std::cerr << "Failed to get graphics queue. Error: " << graphics_queue_ret.error().message() << "\n";
        return FAIL;
    }
    VkQueue graphics_queue = graphics_queue_ret.value ();

    return SUCCESS;
}

VkEngine::~VkEngine() {}