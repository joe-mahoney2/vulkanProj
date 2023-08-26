#include "VkEngine.h"

VkEngine::VkEngine() {
    if (initGlfw()) { std::cout << "VkEngine::initGlfw SUCCESS!" << std::endl; }
    if (initVulkan()) { std::cout << "VkEngine::initVulkan SUCCESS!" << std::endl; }
    if (initRenderData()) { std::cout << "VkEngine::initRenderData SUCCESS!" << std::endl; }
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

    auto graphics_queue_ret =  vkCtl.vkbDevice.get_queue (vkb::QueueType::graphics);
    if (!graphics_queue_ret) {
        std::cerr << "Failed to get graphics queue. Error: " << graphics_queue_ret.error().message() << "\n";
        return FAIL;
    }
    // initialize graphics queue
    VkQueue graphics_queue = graphics_queue_ret.value ();

    vkb::SwapchainBuilder swapchain_builder{vkCtl.vkbDevice};
	auto swap_ret = swapchain_builder.set_old_swapchain(vkCtl.swapchain).build();
	if (!swap_ret) {
		std::cout << swap_ret.error().message() << " " << swap_ret.vk_result() << "\n";
        return FAIL;
	}
    vkb::destroy_swapchain(vkCtl.swapchain);
    // initialize swapchain
	vkCtl.swapchain = swap_ret.value ();
	return SUCCESS;
}

int VkEngine::initRenderData() {
	auto gq = vkCtl.vkbDevice.get_queue (vkb::QueueType::graphics);
	if (!gq.has_value ()) {
		std::cout << "failed to get graphics queue: " << gq.error ().message () << "\n";
		return FAIL;
	}
    vkRen.graphics_queue = gq.value();

	auto pq = vkCtl.vkbDevice.get_queue (vkb::QueueType::present);
	if (!pq.has_value ()) {
		std::cout << "failed to get present queue: " << pq.error ().message () << "\n";
		return -1;
	}
	vkRen.present_queue = pq.value ();

	VkAttachmentDescription color_attachment = {};
	color_attachment.format = vkCtl.swapchain.image_format;
	color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference color_attachment_ref = {};
	color_attachment_ref.attachment = 0;
	color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment_ref;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    // above all for setting up the render pass
	VkRenderPassCreateInfo render_pass_info = {};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.attachmentCount = 1;
	render_pass_info.pAttachments = &color_attachment;
	render_pass_info.subpassCount = 1;
	render_pass_info.pSubpasses = &subpass;
	render_pass_info.dependencyCount = 1;
	render_pass_info.pDependencies = &dependency;

	if (vkCreateRenderPass (vkCtl.vkbDevice, &render_pass_info, nullptr, &vkRen.renderPass) != VK_SUCCESS) {
		std::cout << "failed to create render pass\n";
		return FAIL;
	}
    return SUCCESS;
}

VkEngine::~VkEngine() {}