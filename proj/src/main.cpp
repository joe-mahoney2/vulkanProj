#define GLFW_INCLUDE_VULKAN

#include<iostream>
#include<vector>
#include <fstream>
#include <string>

#include <GLFW/glfw3.h>
#include <VkBootstrap.h>
#include <VkBootstrapDispatch.h>

#include "globalDefs.h"

#define SUCCESS 1
#define FAIL 0

const int MAX_FRAMES_IN_FLIGHT = 2;

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


std::vector<char> readFile (const std::string& filename) {
	std::ifstream file (filename, std::ios::ate | std::ios::binary);

	if (!file.is_open ()) {
		throw std::runtime_error ("failed to open file!");
	}

	size_t file_size = (size_t)file.tellg ();
	std::vector<char> buffer (file_size);

	file.seekg (0);
	file.read (buffer.data (), static_cast<std::streamsize> (file_size));

	file.close ();

	return buffer;
}


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

VkShaderModule createShaderModule (const std::vector<char>& code) {
	VkShaderModuleCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create_info.codeSize = code.size ();
	create_info.pCode = reinterpret_cast<const uint32_t*> (code.data ());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(vkCtl.device, &create_info, nullptr, &shaderModule) != VK_SUCCESS) {
		return VK_NULL_HANDLE; // failed to create shader module
	}

	return shaderModule;
}

int initGraphicsPipe() {
	auto vert_code = readFile(std::string(SHADER_PATH) + "/vert.spv");
	auto frag_code = readFile(std::string(SHADER_PATH) + "/frag.spv");

	VkShaderModule vert_module = createShaderModule(vert_code);
	VkShaderModule frag_module = createShaderModule(frag_code);
	if (vert_module == VK_NULL_HANDLE || frag_module == VK_NULL_HANDLE) {
		std::cout << "failed to create shader module\n";
		return FAIL;
	}

	VkPipelineShaderStageCreateInfo vert_stage_info = {};
	vert_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vert_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vert_stage_info.module = vert_module;
	vert_stage_info.pName = "main";

	VkPipelineShaderStageCreateInfo frag_stage_info = {};
	frag_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	frag_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	frag_stage_info.module = frag_module;
	frag_stage_info.pName = "main";

	VkPipelineShaderStageCreateInfo shader_stages[] = { vert_stage_info, frag_stage_info };

	VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
	vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_info.vertexBindingDescriptionCount = 0;
	vertex_input_info.vertexAttributeDescriptionCount = 0;

	VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
	input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)vkCtl.swapchain.extent.width;
	viewport.height = (float)vkCtl.swapchain.extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = vkCtl.swapchain.extent;

	VkPipelineViewportStateCreateInfo viewport_state = {};
	viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_state.viewportCount = 1;
	viewport_state.pViewports = &viewport;
	viewport_state.scissorCount = 1;
	viewport_state.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
	                                      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo color_blending = {};
	color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blending.logicOpEnable = VK_FALSE;
	color_blending.logicOp = VK_LOGIC_OP_COPY;
	color_blending.attachmentCount = 1;
	color_blending.pAttachments = &colorBlendAttachment;
	color_blending.blendConstants[0] = 0.0f;
	color_blending.blendConstants[1] = 0.0f;
	color_blending.blendConstants[2] = 0.0f;
	color_blending.blendConstants[3] = 0.0f;

	VkPipelineLayoutCreateInfo pipeline_layout_info = {};
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_info.setLayoutCount = 0;
	pipeline_layout_info.pushConstantRangeCount = 0;

	if (vkCreatePipelineLayout (
	        vkCtl.device, &pipeline_layout_info, nullptr, &renderData.pipeline_layout) != VK_SUCCESS) {
		std::cout << "failed to create pipeline layout\n";
		return FAIL;
	}

	std::vector<VkDynamicState> dynamic_states = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	VkPipelineDynamicStateCreateInfo dynamic_info = {};
	dynamic_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_info.dynamicStateCount = static_cast<uint32_t> (dynamic_states.size ());
	dynamic_info.pDynamicStates = dynamic_states.data ();	

	VkGraphicsPipelineCreateInfo pipeline_info = {};
	pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_info.stageCount = 2;
	pipeline_info.pStages = shader_stages;
	pipeline_info.pVertexInputState = &vertex_input_info;
	pipeline_info.pInputAssemblyState = &input_assembly;
	pipeline_info.pViewportState = &viewport_state;
	pipeline_info.pRasterizationState = &rasterizer;
	pipeline_info.pMultisampleState = &multisampling;
	pipeline_info.pColorBlendState = &color_blending;
	pipeline_info.pDynamicState = &dynamic_info;
	pipeline_info.layout = renderData.pipeline_layout;
	pipeline_info.renderPass = renderData.render_pass;
	pipeline_info.subpass = 0;
	pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

	if (vkCreateGraphicsPipelines (
	        vkCtl.device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &renderData.graphics_pipeline) != VK_SUCCESS) {
		std::cout << "failed to create pipline\n";
		return FAIL; // failed to create graphics pipeline
	}

	vkDestroyShaderModule (vkCtl.device, frag_module, nullptr);
	vkDestroyShaderModule (vkCtl.device, vert_module, nullptr);

	return SUCCESS;
}

int initFrameBuffers () {
	renderData.swapchain_images = vkCtl.swapchain.get_images ().value ();
	renderData.swapchain_image_views = vkCtl.swapchain.get_image_views ().value ();

	renderData.framebuffers.resize (renderData.swapchain_image_views.size ());

	for (size_t i = 0; i < renderData.swapchain_image_views.size (); i++) {
		VkImageView attachments[] = { renderData.swapchain_image_views[i] };

		VkFramebufferCreateInfo framebuffer_info = {};
		framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_info.renderPass = renderData.render_pass;
		framebuffer_info.attachmentCount = 1;
		framebuffer_info.pAttachments = attachments;
		framebuffer_info.width = vkCtl.swapchain.extent.width;
		framebuffer_info.height = vkCtl.swapchain.extent.height;
		framebuffer_info.layers = 1;

		if (vkCreateFramebuffer (vkCtl.device, &framebuffer_info, nullptr, &renderData.framebuffers[i]) != VK_SUCCESS) {
			return FAIL; // failed to create framebuffer
		}
	}

	return SUCCESS;
}

int initCommandPool () {
	VkCommandPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.queueFamilyIndex = vkCtl.device.get_queue_index (vkb::QueueType::graphics).value ();

	if (vkCreateCommandPool (vkCtl.device, &pool_info, nullptr, &renderData.command_pool) != VK_SUCCESS) {
		std::cout << "failed to create command pool\n";
		return FAIL; // failed to create command pool
	}
	return SUCCESS;
}

int initCommandBuffers () {	
	renderData.command_buffers.resize (renderData.framebuffers.size ());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = renderData.command_pool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)renderData.command_buffers.size ();

	if (vkAllocateCommandBuffers (vkCtl.device, &allocInfo, renderData.command_buffers.data ()) != VK_SUCCESS) {
		return FAIL; // failed to allocate command buffers;
	}

	for (size_t i = 0; i < renderData.command_buffers.size (); i++) {
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer (renderData.command_buffers[i], &begin_info) != VK_SUCCESS) {
			return FAIL; // failed to begin recording command buffer
		}

		VkRenderPassBeginInfo render_pass_info = {};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_info.renderPass = renderData.render_pass;
		render_pass_info.framebuffer = renderData.framebuffers[i];
		render_pass_info.renderArea.offset = { 0, 0 };
		render_pass_info.renderArea.extent = vkCtl.swapchain.extent;
		VkClearValue clearColor{ { { 0.0f, 0.0f, 0.0f, 1.0f } } };
		render_pass_info.clearValueCount = 1;
		render_pass_info.pClearValues = &clearColor;

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)vkCtl.swapchain.extent.width;
		viewport.height = (float)vkCtl.swapchain.extent.height;
		viewport.minDepth = 0.0f;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = vkCtl.swapchain.extent;

		vkCmdSetViewport (renderData.command_buffers[i], 0, 1, &viewport);
		vkCmdSetScissor (renderData.command_buffers[i], 0, 1, &scissor);

		vkCmdBeginRenderPass (renderData.command_buffers[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline (renderData.command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, renderData.graphics_pipeline);

		vkCmdDraw (renderData.command_buffers[i], 3, 1, 0, 0);

		vkCmdEndRenderPass (renderData.command_buffers[i]);

		if (vkEndCommandBuffer (renderData.command_buffers[i]) != VK_SUCCESS) {
			std::cout << "failed to record command buffer\n";
			return FAIL; // failed to record command buffer!
		}
	}
	return SUCCESS;
}

int initSyncObjects() {
	renderData.available_semaphores.resize (MAX_FRAMES_IN_FLIGHT);
	renderData.finished_semaphore.resize (MAX_FRAMES_IN_FLIGHT);
	renderData.in_flight_fences.resize (MAX_FRAMES_IN_FLIGHT);
	renderData.image_in_flight.resize (vkCtl.swapchain.image_count, VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphore_info = {};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fence_info = {};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore (vkCtl.device, &semaphore_info, nullptr, &renderData.available_semaphores[i]) != VK_SUCCESS ||
		    vkCreateSemaphore (vkCtl.device, &semaphore_info, nullptr, &renderData.finished_semaphore[i]) != VK_SUCCESS ||
		    vkCreateFence (vkCtl.device, &fence_info, nullptr, &renderData.in_flight_fences[i]) != VK_SUCCESS) {
			std::cout << "failed to create sync objects\n";
			return FAIL; // failed to create synchronization objects for a frame
		}
	}

	return SUCCESS;
}

int recreate_swapchain () {
	vkDeviceWaitIdle (vkCtl.device);

	vkDestroyCommandPool (vkCtl.device, renderData.command_pool, nullptr);

	for (auto framebuffer : renderData.framebuffers) {
		vkDestroyFramebuffer (vkCtl.device, framebuffer, nullptr);
	}

	vkCtl.swapchain.destroy_image_views (renderData.swapchain_image_views);

    if (SUCCESS != initSwapChain ()) return FAIL;
	if (SUCCESS != initFrameBuffers ()) return FAIL;
	if (SUCCESS != initCommandPool ()) return FAIL;
	if (SUCCESS != initCommandBuffers ()) return FAIL;
	return SUCCESS;
}

int drawFrame () {
	vkWaitForFences (vkCtl.device, 1, &renderData.in_flight_fences[renderData.current_frame], VK_TRUE, UINT64_MAX);

	uint32_t image_index = 0;
	VkResult result = vkAcquireNextImageKHR (vkCtl.device,
	    vkCtl.swapchain,
	    UINT64_MAX,
	    renderData.available_semaphores[renderData.current_frame],
	    VK_NULL_HANDLE,
	    &image_index);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		return recreate_swapchain ();
	} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		std::cout << "failed to acquire swapchain image. Error " << result << "\n";
		return FAIL;
	}

	if (renderData.image_in_flight[image_index] != VK_NULL_HANDLE) {
		vkWaitForFences (vkCtl.device, 1, &renderData.image_in_flight[image_index], VK_TRUE, UINT64_MAX);
	}
	renderData.image_in_flight[image_index] = renderData.in_flight_fences[renderData.current_frame];

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore wait_semaphores[] = { renderData.available_semaphores[renderData.current_frame] };
	VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = wait_semaphores;
	submitInfo.pWaitDstStageMask = wait_stages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &renderData.command_buffers[image_index];

	VkSemaphore signal_semaphores[] = { renderData.finished_semaphore[renderData.current_frame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signal_semaphores;

	vkResetFences (vkCtl.device, 1, &renderData.in_flight_fences[renderData.current_frame]);

	if (vkQueueSubmit (renderData.graphics_queue, 1, &submitInfo, renderData.in_flight_fences[renderData.current_frame]) != VK_SUCCESS) {
		std::cout << "failed to submit draw command buffer\n";
		return FAIL; //"failed to submit draw command buffer
	}

	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = signal_semaphores;

	VkSwapchainKHR swapChains[] = { vkCtl.swapchain };
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swapChains;

	present_info.pImageIndices = &image_index;

	result = vkQueuePresentKHR (renderData.present_queue, &present_info);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		return recreate_swapchain ();
	} else if (result != VK_SUCCESS) {
		std::cout << "failed to present swapchain image\n";
		return FAIL;
	}

	renderData.current_frame = (renderData.current_frame + 1) % MAX_FRAMES_IN_FLIGHT;

	return SUCCESS;
}

int main() {

	if (initWindow()) { std::cout << "success::initWindow" << std::endl; }
	if (initVulkan()) { std::cout << "success::initVulkan" << std::endl; }
	if (initSwapChain()) { std::cout << "success::initSwapChain" << std::endl; }
	if (initRenderData()) { std::cout << "success::initRenderData" << std::endl; }
	if (initGraphicsPipe()) { std::cout << "success::initGraphicsPipe" << std::endl; }
	if (initFrameBuffers()) { std::cout << "success::initFrameBuffers" << std::endl; }
	if (initCommandPool()) { std::cout << "success::initCommandPool" << std::endl; }
	if (initCommandBuffers()) { std::cout << "success::initCommandBuffers" << std::endl; }
	if (initSyncObjects()) { std::cout << "success::initSyncObjects" << std::endl; }

	while (!glfwWindowShouldClose(vkCtl.window)) {
		glfwPollEvents();
		int status = drawFrame();
		if(status != SUCCESS) { 
			std::cout << "epic fail bro" << std::endl;  
			return FAIL;
		}
	}
	vkDeviceWaitIdle(vkCtl.device);
	return 0;
	}