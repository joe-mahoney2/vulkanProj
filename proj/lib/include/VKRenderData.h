#pragma once 


#include "VkBootstrap.h"
#include "GLMDefs.h"
#include "globalDefs.h"


typedef struct RenderData {
    VkQueue gfxQueue;
    VkQueue present_queue;
    std::vector<VkImage> scImages;
    std::vector<VkImageView> scImageViews;
    std::vector<VkFramebuffer> frameBuffers;
    VkRenderPass renderPass;
    VkPipelineLayout pipeLayout;
    VkPipeline gfxPipe;
    VkCommandPool cmdPool;
    std::vector<VkCommandBuffer> cmdBuffers;
    std::vector<VkSemaphore> available_semaphores;
    std::vector<VkSemaphore> finished_semaphore;
    std::vector<VkFence> in_flight_fences;
    std::vector<VkFence> image_in_flight;
    VkVertexInputBindingDescription bindingDescription;
    VkPipelineVertexInputStateCreateInfo vertexInputInfo;
    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions;
    VkMemoryRequirements memRequirements;
    VkPhysicalDeviceMemoryProperties memProperties;
    VkBufferCreateInfo bufferInfo;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    VkDescriptorSetLayout descriptorSetLayout;
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;
    size_t current_frame = 0;
} RenderData_t;

inline int initBindingDescription(RenderData_t* render_data) {

    render_data->bindingDescription.binding = 0;
    render_data->bindingDescription.stride = sizeof(Vertex);
    render_data->bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    // float: VK_FORMAT_R32_SFLOAT
    // vec2: VK_FORMAT_R32G32_SFLOAT
    // vec3: VK_FORMAT_R32G32B32_SFLOAT
    // vec4: VK_FORMAT_R32G32B32A32_SFLOAT
    render_data->attributeDescriptions[0].binding = 0;
    render_data->attributeDescriptions[0].location = 0;
    render_data->attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    render_data->attributeDescriptions[0].offset = offsetof(Vertex, pos);

    render_data->attributeDescriptions[1].binding = 0;
    render_data->attributeDescriptions[1].location = 1;
    render_data->attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    render_data->attributeDescriptions[1].offset = offsetof(Vertex, color);

    render_data->attributeDescriptions[2].binding = 0;
    render_data->attributeDescriptions[2].location = 2;
    render_data->attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    render_data->attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

    render_data->vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    render_data->vertexInputInfo.vertexBindingDescriptionCount = 1;
    render_data->vertexInputInfo.pVertexBindingDescriptions = &render_data->bindingDescription;
    render_data->vertexInputInfo.pVertexAttributeDescriptions = render_data->attributeDescriptions.data();
    render_data->vertexInputInfo.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(render_data->attributeDescriptions.size());

    return SUCCESS;
}

inline uint32_t findVkMemoryType(RenderData_t* render_data, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    for (uint32_t i = 0; i < render_data->memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (render_data->memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    return FAIL;
}