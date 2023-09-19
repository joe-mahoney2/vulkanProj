#include "VkEngine.h"

VkEngine::VkEngine() {
    if (initGlfw()) { 
		std::cout << "VkEngine::initGlfw SUCCESS!" << std::endl; 
	}
    if (initVulkan()) { 
		std::cout << "VkEngine::initVulkan SUCCESS!" << std::endl; 
	}
	if (initSwapChain()) { 
		std::cout << "VkEngine::initSwapChain SUCCESS!" << std::endl; 
	}
	if(initVertexBuffers()) {
		std::cout << "VkEngine::initVertexBuffers SUCCESS!" << std::endl;
	}
	if(initBindingDescription()) {
		std::cout << "VkEngine::initBindingDescription SUCCESS!" << std::endl;
	}
    if (initRenderData()) { 
		std::cout << "VkEngine::initRenderData SUCCESS!" << std::endl; 
	}
	if (initGfxPipeline()) { 
		std::cout << "VkEngine::initGfxPipeline SUCCESS!" << std::endl; 
	}
	if (initFrameBuffer()) { 
		std::cout << "VkEngine::initFrameBuffer SUCCESS!" << std::endl; 
	}
	if (initCmdPool()) { 
		std::cout << "VkEngine::initCmdPool SUCCESS!" << std::endl; 
	}
	if (initCmdBuffer()) { 
		std::cout << "VkEngine::initCmdBuffer SUCCESS!" << std::endl; 
	}
	if (initSyncObjects()) { 
		std::cout << "VkEngine::initSyncObjects SUCCESS!" << std::endl;
	}
}
/*
	Initializer type function definitions beyond this comment
*/
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
        std::cerr << "Failed to create Vulkan instance. Error: " << 
			inst_ret.error().message() << "\n";
        return FAIL;
    }
    // initialized vulkan instance
    vkCtl.vkbInst = inst_ret.value ();
    
    glfwCreateWindowSurface(vkCtl.vkbInst.instance, 
						vkCtl.glfw->window, 
						NULL, 
						&vkCtl.surface);

    vkb::PhysicalDeviceSelector selector{  vkCtl.vkbInst };
    auto phys_ret = 
		selector.set_surface (vkCtl.surface)
                .set_minimum_version (1, 1) // require a vulkan 1.1 capable device
                .require_dedicated_transfer_queue ()
                .select ();

    if (!phys_ret) {
        std::cerr << "Failed to select Vulkan Physical Device. Error: " <<
			phys_ret.error().message() << "\n";
        return FAIL;
    }

    vkb::DeviceBuilder device_builder{ phys_ret.value () };
    auto dev_ret = device_builder.build ();
    if (!dev_ret) {
        std::cerr << "Failed to create Vulkan device. Error: " << 
			dev_ret.error().message() << "\n";
        return FAIL;
    }
    // initialize vulkan device
    vkCtl.vkbDevice = dev_ret.value ();

    auto graphics_queue_ret =  
		vkCtl.vkbDevice.get_queue (vkb::QueueType::graphics);
    if (!graphics_queue_ret) {
        std::cerr << "Failed to get graphics queue. Error: " << 
			graphics_queue_ret.error().message() << "\n";
        return FAIL;
    }
    // initialize graphics queue
    vkCtl.gfxQueue = graphics_queue_ret.value ();

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

int VkEngine::initSwapChain() {
    vkb::SwapchainBuilder swapchain_builder{vkCtl.vkbDevice};
	auto swap_ret = 
		swapchain_builder.set_old_swapchain(vkCtl.swapchain).build();
	if (!swap_ret) {
		std::cout << swap_ret.error().message() << " " << 
			swap_ret.vk_result() << "\n";
        return FAIL;
	}
    vkb::destroy_swapchain(vkCtl.swapchain);
	vkCtl.swapchain = swap_ret.value ();
	return SUCCESS;
}

int VkEngine::initRenderData() {
	auto gq = vkCtl.vkbDevice.get_queue (vkb::QueueType::graphics);
	if (!gq.has_value ()) {
		std::cout << "failed to get graphics queue: " << 
			gq.error ().message () << "\n";
		return FAIL;
	}
    vkRen.gfxQueue = gq.value();

	auto pq = vkCtl.vkbDevice.get_queue (vkb::QueueType::present);
	if (!pq.has_value ()) {
		std::cout << "failed to get present queue: " << 
			pq.error ().message () << "\n";
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
	dependency.dstAccessMask = 
		VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | 
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    // above all for setting up the render pass
	VkRenderPassCreateInfo render_pass_info = {};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.attachmentCount = 1;
	render_pass_info.pAttachments = &color_attachment;
	render_pass_info.subpassCount = 1;
	render_pass_info.pSubpasses = &subpass;
	render_pass_info.dependencyCount = 1;
	render_pass_info.pDependencies = &dependency;

	if (vkCreateRenderPass (vkCtl.vkbDevice, 
						&render_pass_info, 
						nullptr, 
						&vkRen.renderPass) 
						!= VK_SUCCESS) {
		std::cout << "failed to create render pass\n";
		return FAIL;
	}
    return SUCCESS;
}

int VkEngine::initGfxPipeline() {
	auto vert_code = readFile(std::string(SHADER_PATH) + "/vert.spv");
	auto frag_code = readFile(std::string(SHADER_PATH) + "/frag.spv");

	VkShaderModule vert_module = createShaderModule(vert_code);
	VkShaderModule frag_module = createShaderModule(frag_code);
	if (vert_module == VK_NULL_HANDLE || frag_module == VK_NULL_HANDLE) {
		std::cout << "failed to create shader module\n";
		return FAIL;
	}

	VkPipelineShaderStageCreateInfo vert_stage_info = {};
	vert_stage_info.sType = 
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vert_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vert_stage_info.module = vert_module;
	vert_stage_info.pName = "main";

	VkPipelineShaderStageCreateInfo frag_stage_info = {};
	frag_stage_info.sType = 
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	frag_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	frag_stage_info.module = frag_module;
	frag_stage_info.pName = "main";

	VkPipelineShaderStageCreateInfo shader_stages[] = 
		{ 
			vert_stage_info, 
			frag_stage_info 
		};


												VkVertexInputBindingDescription bindingDescription{};
												bindingDescription.binding = 0;
												bindingDescription.stride = sizeof(Vertex);
												bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

												VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
												vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
												vertex_input_info.vertexBindingDescriptionCount = 1;
												vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(vkRen.attributeDescriptions.size());
												vertex_input_info.pVertexAttributeDescriptions = vkRen.attributeDescriptions.data();;
												vertex_input_info.pVertexBindingDescriptions = &bindingDescription;





	VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
	input_assembly.sType = 
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
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
	viewport_state.sType = 
		VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_state.viewportCount = 1;
	viewport_state.pViewports = &viewport;
	viewport_state.scissorCount = 1;
	viewport_state.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = 
		VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = 
		VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = 
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
	    VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;		
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo color_blending = {};
	color_blending.sType = 
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
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

	if (vkCreatePipelineLayout (vkCtl.vkbDevice, 
							&pipeline_layout_info, 
							nullptr, 
							&vkRen.pipeLayout) 
							!= VK_SUCCESS) {
		std::cout << "failed to create pipeline layout\n";
		return FAIL;
	}

	std::vector<VkDynamicState> dynamic_states = 
		{ 
			VK_DYNAMIC_STATE_VIEWPORT, 
			VK_DYNAMIC_STATE_SCISSOR 
		};

	VkPipelineDynamicStateCreateInfo dynamic_info = {};
	dynamic_info.sType = 
		VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_info.dynamicStateCount = 
		static_cast<uint32_t> (dynamic_states.size ());
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
	pipeline_info.layout = vkRen.pipeLayout;
	pipeline_info.renderPass = vkRen.renderPass;
	pipeline_info.subpass = 0;
	pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

	if (vkCreateGraphicsPipelines(vkCtl.vkbDevice, 
								VK_NULL_HANDLE, 
								1, 
								&pipeline_info, 
								nullptr, 
								&vkRen.gfxPipe) 
								!= VK_SUCCESS) {

		std::cout << "failed to create pipline\n";
		return FAIL; // failed to create graphics pipeline
	}

	vkDestroyShaderModule(vkCtl.vkbDevice, frag_module, nullptr);
	vkDestroyShaderModule(vkCtl.vkbDevice, vert_module, nullptr);

	return SUCCESS;
}

int VkEngine::initFrameBuffer() {
	vkRen.scImages = vkCtl.swapchain.get_images().value();
	vkRen.scImageViews = vkCtl.swapchain.get_image_views().value();

	vkRen.frameBuffers.resize (vkRen.scImageViews.size());

	for (size_t i = 0; i < vkRen.scImageViews.size(); i++) {
		VkImageView attachments[] = { vkRen.scImageViews[i] };

		VkFramebufferCreateInfo framebuffer_info = {};
		framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_info.renderPass = vkRen.renderPass;
		framebuffer_info.attachmentCount = 1;
		framebuffer_info.pAttachments = attachments;
		framebuffer_info.width = vkCtl.swapchain.extent.width;
		framebuffer_info.height = vkCtl.swapchain.extent.height;
		framebuffer_info.layers = 1;

		if (vkCreateFramebuffer (vkCtl.vkbDevice, 
								&framebuffer_info, 
								nullptr, 
								&vkRen.frameBuffers[i]) 
								!= VK_SUCCESS) {
			return FAIL;
		}
	}
	return SUCCESS;
}

int VkEngine::initCmdPool() {
	VkCommandPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

	pool_info.queueFamilyIndex = 
		vkCtl.vkbDevice.get_queue_index (vkb::QueueType::graphics).value ();

	if (vkCreateCommandPool (vkCtl.vkbDevice, 
							&pool_info, nullptr, 
							&vkRen.cmdPool) 
							!= VK_SUCCESS) {
		std::cout << "failed to create command pool\n";
		return FAIL; // failed to create command pool
	}
	return SUCCESS;
}

int VkEngine::initCmdBuffer() {
	vkRen.cmdBuffers.resize (vkRen.frameBuffers.size ());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = vkRen.cmdPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)vkRen.cmdBuffers.size ();

	if (vkAllocateCommandBuffers (vkCtl.vkbDevice, 
								&allocInfo, 
								vkRen.cmdBuffers.data ()) 
								!= VK_SUCCESS) {
		return FAIL; // failed to allocate command buffers;
	}

	for (size_t i = 0; i < vkRen.cmdBuffers.size (); i++) {
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer (vkRen.cmdBuffers[i], 
								&begin_info) 
								!= VK_SUCCESS) {
			return FAIL; // failed to begin recording command buffer
		}

		VkClearValue clearColor{ { { 0.0f, 0.0f, 0.0f, 1.0f } } };

		VkRenderPassBeginInfo render_pass_info = {};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_info.renderPass = vkRen.renderPass;
		render_pass_info.framebuffer = vkRen.frameBuffers[i];
		render_pass_info.renderArea.offset = { 0, 0 };
		render_pass_info.renderArea.extent = vkCtl.swapchain.extent;
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

		vkCmdSetViewport (vkRen.cmdBuffers[i], 0, 1, &viewport);
		vkCmdSetScissor (vkRen.cmdBuffers[i], 0, 1, &scissor);

										VkBufferCreateInfo bufferInfo{};
										bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
										bufferInfo.size = sizeof(vertices[0]) * vertices.size();
										bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
										bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

										void* data;
										vkMapMemory(vkCtl.vkbDevice, vkRen.vertexBufferMemory, 0, bufferInfo.size, 0, &data);
										memcpy(data, vertices.data(), (size_t) bufferInfo.size);
										vkUnmapMemory(vkCtl.vkbDevice, vkRen.vertexBufferMemory);

										VkBuffer vertexBuffers[] = {vkRen.vertexBuffer};
										VkDeviceSize offsets[] = {0};
										vkCmdBindVertexBuffers(vkRen.cmdBuffers[i], 0, 1, vertexBuffers, offsets);
										vkCmdBindPipeline(vkRen.cmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vkRen.gfxPipe);
										vkCmdBeginRenderPass (vkRen.cmdBuffers[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

										vkCmdDraw(vkRen.cmdBuffers[i], static_cast<uint32_t>(vertices.size()), 1, 0, 0);
										vkCmdEndRenderPass (vkRen.cmdBuffers[i]);

		if (vkEndCommandBuffer (vkRen.cmdBuffers[i]) != VK_SUCCESS) {
			std::cout << "failed to record command buffer\n";
			return FAIL; // failed to record command buffer!
		}
	}
	return SUCCESS;
}

int VkEngine::initSyncObjects() {
	vkRen.available_semaphores.resize (MAX_FRAMES_IN_FLIGHT);
	vkRen.finished_semaphore.resize (MAX_FRAMES_IN_FLIGHT);
	vkRen.in_flight_fences.resize (MAX_FRAMES_IN_FLIGHT);
	vkRen.image_in_flight.resize (vkCtl.swapchain.image_count, VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphore_info = {};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fence_info = {};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore (vkCtl.vkbDevice, &semaphore_info, nullptr, 
							&vkRen.available_semaphores[i]) != VK_SUCCESS ||
		    vkCreateSemaphore (vkCtl.vkbDevice, &semaphore_info, nullptr, 
							&vkRen.finished_semaphore[i]) != VK_SUCCESS ||
		    vkCreateFence (vkCtl.vkbDevice, &fence_info, nullptr, 
							&vkRen.in_flight_fences[i]) != VK_SUCCESS) {

			std::cout << "failed to create sync objects\n";
			return FAIL; // failed to create synchronization objects for a frame
		}
	}

	return SUCCESS;
}

/*
	Drawing related functions beyond this comment
*/
int VkEngine::recreateSwapChain() {
	vkDeviceWaitIdle (vkCtl.vkbDevice);

	vkDestroyCommandPool (vkCtl.vkbDevice, 
						vkRen.cmdPool, 
						nullptr);

	for (auto framebuffer : vkRen.frameBuffers) {
		vkDestroyFramebuffer (vkCtl.vkbDevice, 
							framebuffer, 
							nullptr);
	}

	vkCtl.swapchain.destroy_image_views (vkRen.scImageViews);

    if (SUCCESS != initSwapChain ()) return FAIL;
	if (SUCCESS != initFrameBuffer ()) return FAIL;
	if (SUCCESS != initCmdPool ()) return FAIL;
	if (SUCCESS != initCmdBuffer ()) return FAIL;
	return SUCCESS;
}

int VkEngine::drawFrame() {
	uint32_t image_index = 0;
	vkWaitForFences (vkCtl.vkbDevice, 
					1, 
					&vkRen.in_flight_fences[vkRen.current_frame], 
					VK_TRUE, 
					UINT64_MAX);

	VkResult result = 
	vkAcquireNextImageKHR (vkCtl.vkbDevice,
						vkCtl.swapchain,
						UINT64_MAX,
						vkRen.available_semaphores[vkRen.current_frame],
						VK_NULL_HANDLE,
						&image_index);

	if (vkRen.image_in_flight[image_index] != VK_NULL_HANDLE) {
		vkWaitForFences (vkCtl.vkbDevice, 
						1,
						&vkRen.image_in_flight[image_index], 
						VK_TRUE, 
						UINT64_MAX);
	}
	vkRen.image_in_flight[image_index] = 
		vkRen.in_flight_fences[vkRen.current_frame];

	VkSemaphore wait_semaphores[] = { 
		vkRen.available_semaphores[vkRen.current_frame] };
	VkSemaphore signal_semaphores[] = { 
		vkRen.finished_semaphore[vkRen.current_frame] };
	VkPipelineStageFlags wait_stages[] = { 
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = wait_semaphores;
	submitInfo.pWaitDstStageMask = wait_stages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &vkRen.cmdBuffers[image_index];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signal_semaphores;

	vkResetFences (vkCtl.vkbDevice, 
				1, 
				&vkRen.in_flight_fences[vkRen.current_frame]);

	if (vkQueueSubmit (vkRen.gfxQueue, 
					1, 
					&submitInfo, 
					vkRen.in_flight_fences[vkRen.current_frame]) 
					!= VK_SUCCESS) {

		std::cout << "failed to submit draw command buffer\n";
		return FAIL;
	}

	VkSwapchainKHR swapChains[] = { vkCtl.swapchain };

	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = signal_semaphores;
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swapChains;
	present_info.pImageIndices = &image_index;

	result = vkQueuePresentKHR (vkRen.present_queue, &present_info);
	vkRen.current_frame = (vkRen.current_frame + 1) % MAX_FRAMES_IN_FLIGHT;

	return SUCCESS;
}

/*
	Helper type functions beyond this comment
*/
VkShaderModule VkEngine::createShaderModule(const std::vector<char>& code) {
	VkShaderModuleCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create_info.codeSize = code.size ();
	create_info.pCode = reinterpret_cast<const uint32_t*> (code.data ());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(vkCtl.vkbDevice, 
							&create_info, 
							nullptr, 
							&shaderModule) 
							!= VK_SUCCESS) {
		return VK_NULL_HANDLE; // failed to create shader module
	}
	return shaderModule;
}

int VkEngine::initBindingDescription() {
	VkVertexInputBindingDescription bindingDescription{};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	// float: VK_FORMAT_R32_SFLOAT
	// vec2: VK_FORMAT_R32G32_SFLOAT
	// vec3: VK_FORMAT_R32G32B32_SFLOAT
	// vec4: VK_FORMAT_R32G32B32A32_SFLOAT
	vkRen.attributeDescriptions[0].binding = 0;
	vkRen.attributeDescriptions[0].location = 0;
	vkRen.attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	vkRen.attributeDescriptions[0].offset = offsetof(Vertex, pos);

	vkRen.attributeDescriptions[1].binding = 0;
	vkRen.attributeDescriptions[1].location = 1;
	vkRen.attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	vkRen.attributeDescriptions[1].offset = offsetof(Vertex, color);

	
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = 
		static_cast<uint32_t>(vkRen.attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = 
		vkRen.attributeDescriptions.data();

	return SUCCESS;
}

int VkEngine::initVertexBuffers() {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(vertices[0]) * vertices.size();
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(vkCtl.vkbDevice, &bufferInfo, nullptr, &vkRen.vertexBuffer) != VK_SUCCESS) {
        return FAIL;
    }

	vkGetBufferMemoryRequirements(vkCtl.vkbDevice, vkRen.vertexBuffer, &vkRen.memRequirements);
	vkGetPhysicalDeviceMemoryProperties(vkCtl.vkbDevice.physical_device, &vkRen.memProperties);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = vkRen.memRequirements.size;

	if(!(allocInfo.memoryTypeIndex = 
		findVkMemoryType(vkRen.memRequirements.memoryTypeBits, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))) {

		std::cout << "findVkMemoryType failed" << std::endl;
		return FAIL;
	}

	if (vkAllocateMemory(vkCtl.vkbDevice, 
		&allocInfo, 
		nullptr, 
		&vkRen.vertexBufferMemory) != VK_SUCCESS) {

			std::cout << "failed to allocate vertex buffer memory!" << std::endl;
			return FAIL;
	}

	if(vkBindBufferMemory(vkCtl.vkbDevice, vkRen.vertexBuffer, 
		vkRen.vertexBufferMemory, 0) != VK_SUCCESS) {
			std::cout << "vertex buffer bound failed!" << std::endl;
	}

	return SUCCESS;
}

VkEngine::~VkEngine() {
	vkDestroyBuffer(vkCtl.vkbDevice, vkRen.vertexBuffer, nullptr);
	vkFreeMemory(vkCtl.vkbDevice, vkRen.vertexBufferMemory, nullptr);
}

/*
	Vulkan helper functions defined here
		findVkMemoryType() -
*/

uint32_t VkEngine::findVkMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	for (uint32_t i = 0; i < vkRen.memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (vkRen.memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
    }
	return FAIL;
}