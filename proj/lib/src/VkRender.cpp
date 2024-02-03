#include "VkRender.h"

VkRender::VkRender() {
    if (initGlfw() == FAIL) { error_msg("initGlfw()"); }
    if (initVulkanBootstrap() == FAIL) { error_msg("initVulkanBootstrap()"); }
    if (initVulkanRenderer() == FAIL) { error_msg("initVulkanRenderData()"); }
}

/*
    Initialize GlfwInterface
    -handles user input and window generation and management
*/
int VkRender::initGlfw() {
    int status = SUCCESS;
    vulkan.glfw = new GlfwInterface();
    if (vulkan.glfw->window == NULL) { status = FAIL; }
    return status;
}

/*
    Initialize Vulkan
    -initializes vulkan bootstrap modules
*/
int VkRender::initVulkanBootstrap() {
    int status = FAIL ||
        initVKB(&vulkan) ||
        initVKBSwapChain(&vulkan);

    return status;
}

/*
    Initialize Vulkan Render structures
*/
int VkRender::initVulkanRenderer() {
    initBindingDescription(&render_data);
    int status = FAIL ||
        initVertexBuffers() ||
        initQueue() ||
        initRenderPass();
    std::cout<<status<<std::endl;
    return status;
}

/*
    initialize vertex buffers
*/
int VkRender::initVertexBuffers() {
    render_data.bufferInfo = {};
    render_data.bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    render_data.bufferInfo.size = sizeof(vertices[0]) * vertices.size();
    render_data.bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    render_data.bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(vulkan.vkbDevice, &render_data.bufferInfo,
        nullptr, &render_data.vertexBuffer) != VK_SUCCESS) {
        return FAIL;
    }
    vkGetBufferMemoryRequirements(vulkan.vkbDevice, render_data.vertexBuffer,
        &render_data.memRequirements);

    vkGetPhysicalDeviceMemoryProperties(vulkan.vkbDevice.physical_device,
        &render_data.memProperties);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = render_data.memRequirements.size;

    if (!(allocInfo.memoryTypeIndex = findVkMemoryType(&render_data,
        render_data.memRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))) {
        error_msg("findVkMemoryType failed");
        return FAIL;
    }

    if (vkAllocateMemory(vulkan.vkbDevice, &allocInfo, nullptr,
        &render_data.vertexBufferMemory) != VK_SUCCESS) {
        error_msg("failed to allocate vertex buffer memory!");
        return FAIL;
    }

    if (vkBindBufferMemory(vulkan.vkbDevice, render_data.vertexBuffer,
        render_data.vertexBufferMemory, 0) != VK_SUCCESS) {
        error_msg("vertex buffer bound failed!");
    }
    return SUCCESS;
}

/*
    initialize graphics queue and present queue
*/
int VkRender::initQueue() {
    auto gq = vulkan.vkbDevice.get_queue(vkb::QueueType::graphics);
    if (!gq.has_value()) {
        error_msg("failed to get graphics queue" + gq.error().message());
        return FAIL;
    }

    render_data.gfxQueue = gq.value();
    auto pq = vulkan.vkbDevice.get_queue(vkb::QueueType::present);
    if (!pq.has_value()) {
        error_msg("failed to get present queue: " + pq.error().message());
        return FAIL;
    }
    render_data.present_queue = pq.value();
    return SUCCESS;
}

/*
    initialize render pass
*/
int VkRender::initRenderPass() {
    VkAttachmentDescription color_attachment = {};
    color_attachment.format = vulkan.swapchain.image_format;
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
    subpass.colorAttachmentCount = 1;
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.pColorAttachments = &color_attachment_ref;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcAccessMask = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
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

    if (vkCreateRenderPass(vulkan.vkbDevice, &render_pass_info,
        nullptr, &render_data.renderPass) != VK_SUCCESS) {
        error_msg("failed to create render pass");
        return FAIL;
    }
    return SUCCESS;
}

/*
    destructor
*/
VkRender::~VkRender() {}