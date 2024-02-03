#pragma once
/*
    VKBootstrap header
*/
#include <VkBootstrap.h>
#include <GlfwInterface.h>

const int MAX_FRAMES_IN_FLIGHT = 2;

typedef struct vulkan_ctl {
    vkb::Instance  vkbInst;    /* vulkan instance */
    vkb::Device    vkbDevice;  /* vulkan physical device */
    VkQueue        gfxQueue;   /* vulkan graphics queue */
    vkb::Swapchain swapchain;  /* vulkan swapchain */
    VkSurfaceKHR   surface;    /* vulkan surface */
    GlfwInterface* glfw;       /* glfw window interface */
} vulkan_ctl_t;

/*
    proform the vulkan / vulkan bootstrap initialization process
*/
inline int initVKB(vulkan_ctl_t* vulkan) {
    vkb::InstanceBuilder builder;
    auto inst_ret = builder.set_app_name("Example Vulkan Application")
        .request_validation_layers()
        .use_default_debug_messenger()
        .build();

    if (!inst_ret) {
        std::cerr << "Failed to create Vulkan instance. Error: " <<
            inst_ret.error().message() << "\n";
        return FAIL;
    }

    vulkan->vkbInst = inst_ret.value();
    glfwCreateWindowSurface(vulkan->vkbInst.instance,
        vulkan->glfw->window,
        NULL,
        &vulkan->surface);

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    vkb::PhysicalDeviceSelector selector{ vulkan->vkbInst };
    auto phys_ret =
        selector.set_surface(vulkan->surface)
        .set_minimum_version(1, 1)
        .set_required_features(deviceFeatures)
        .require_dedicated_transfer_queue()
        .select();

    if (!phys_ret) {
        std::cerr << "Failed to select Vulkan Physical Device. Error: " <<
            phys_ret.error().message() << "\n";
        return FAIL;
    }

    vkb::DeviceBuilder device_builder{ phys_ret.value() };
    auto dev_ret = device_builder.build();
    if (!dev_ret) {
        std::cerr << "Failed to create Vulkan device. Error: " <<
            dev_ret.error().message() << "\n";
        return FAIL;
    }

    vulkan->vkbDevice = dev_ret.value();
    auto graphics_queue_ret =
        vulkan->vkbDevice.get_queue(vkb::QueueType::graphics);
    if (!graphics_queue_ret) {
        std::cerr << "Failed to get graphics queue. Error: " <<
            graphics_queue_ret.error().message() << "\n";
        return FAIL;
    }
    vulkan->gfxQueue = graphics_queue_ret.value();

    vkb::SwapchainBuilder swapchain_builder{ vulkan->vkbDevice };
    auto swap_ret = swapchain_builder.set_old_swapchain(vulkan->swapchain).build();
    if (!swap_ret) {
        std::cout << swap_ret.error().message() << " " << swap_ret.vk_result() << "\n";
        return FAIL;
    }
    vkb::destroy_swapchain(vulkan->swapchain);

    vulkan->swapchain = swap_ret.value();
    return SUCCESS;
}

inline int initVKBSwapChain(vulkan_ctl_t* vulkan) {
    vkb::SwapchainBuilder swapchain_builder{vulkan->vkbDevice};
	auto swap_ret = 
		swapchain_builder.set_old_swapchain(vulkan->swapchain).build();
	if (!swap_ret) {
		std::cout << swap_ret.error().message() << " " << 
			swap_ret.vk_result() << "\n";
        return FAIL;
	}
    vkb::destroy_swapchain(vulkan->swapchain);
	vulkan->swapchain = swap_ret.value ();
	return SUCCESS;   
}