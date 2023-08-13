#define GLFW_INCLUDE_VULKAN

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

struct RenderData {
	VkQueue graphics_queue;
	VkQueue present_queue;

	std::vector<VkImage> swapchain_images;
	std::vector<VkImageView> swapchain_image_views;
	std::vector<VkFramebuffer> framebuffers;

	VkRenderPass render_pass;
	VkPipelineLayout pipeline_layout;
	VkPipeline graphics_pipeline;

	VkCommandPool command_pool;
	std::vector<VkCommandBuffer> command_buffers;

	std::vector<VkSemaphore> available_semaphores;
	std::vector<VkSemaphore> finished_semaphore;
	std::vector<VkFence> in_flight_fences;
	std::vector<VkFence> image_in_flight;
	size_t current_frame = 0;
};

//globals for now class members later
vk_s vkCtl = {};
RenderData renderData;

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

    vkb::InstanceBuilder instance_builder;
    auto instance_ret = instance_builder.use_default_debug_messenger()
                                    .request_validation_layers()
                                    .build();
	if (!instance_ret) {
		std::cout << instance_ret.error().message() << "\n";
		return FAIL;
	}
    //grab instance 
    vkCtl.vkb_inst = instance_ret.value();

    glfwCreateWindowSurface(vkCtl.vkb_inst.instance, vkCtl.window, NULL, &vkCtl.surface);

    vkb::PhysicalDeviceSelector selector{vkCtl.vkb_inst};
    auto phys_ret = selector.set_surface(vkCtl.surface)
                         .set_minimum_version (1, 1) // require a vulkan 1.1 capable device
                         .require_dedicated_transfer_queue ()
                         .select ();
    if (!phys_ret) {
        std::cout << "Failed to select Vulkan Physical Device. Error: " << phys_ret.error().message() << "\n";
        return FAIL;
    }   

    vkb::DeviceBuilder device_builder{phys_ret.value()};
    auto dev_ret = device_builder.build();
    if (!dev_ret) {
        std::cout << "Failed to create Vulkan device. Error: " << dev_ret.error().message() << "\n";
        return FAIL;
    }
    //grab device
    vkCtl.device = dev_ret.value();   

    return SUCCESS;
}

int initSwapChain() {
    vkb::SwapchainBuilder swapchain_builder{vkCtl.device};
	auto swap_ret = swapchain_builder.set_old_swapchain(vkCtl.swapchain).build();
	if (!swap_ret) {
		std::cout << swap_ret.error().message() << " " << swap_ret.vk_result() << "\n";
        return FAIL;
	}
    vkb::destroy_swapchain(vkCtl.swapchain);
	vkCtl.swapchain = swap_ret.value ();
	return SUCCESS;
}

int initRenderData() {

	auto gq = vkCtl.device.get_queue (vkb::QueueType::graphics);
	if (!gq.has_value ()) {
		std::cout << "failed to get graphics queue: " << gq.error ().message () << "\n";
		return FAIL;
	}
	renderData.graphics_queue = gq.value ();

	auto pq = vkCtl.device.get_queue (vkb::QueueType::present);
	if (!pq.has_value ()) {
		std::cout << "failed to get present queue: " << pq.error ().message () << "\n";
		return -1;
	}
	renderData.present_queue = pq.value ();

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

	VkRenderPassCreateInfo render_pass_info = {};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.attachmentCount = 1;
	render_pass_info.pAttachments = &color_attachment;
	render_pass_info.subpassCount = 1;
	render_pass_info.pSubpasses = &subpass;
	render_pass_info.dependencyCount = 1;
	render_pass_info.pDependencies = &dependency;

	if (vkCreateRenderPass (vkCtl.device, &render_pass_info, nullptr, &renderData.render_pass) != VK_SUCCESS) {
		std::cout << "failed to create render pass\n";
		return FAIL;
	}

    return SUCCESS;   
}

int main() {

    if (initWindow()) { std::cout << "success::initWindow" << std::endl;}
    if (initVulkan()) { std::cout << "success::initVulkan" << std::endl;}
    if (initSwapChain()) { std::cout << "success::initSwapChain" << std::endl;}
    if (initRenderData()) { std::cout << "success::initRenderData" << std::endl;}

    while (!glfwWindowShouldClose(vkCtl.window)) {
        glfwPollEvents();
    }

    return 0;
}