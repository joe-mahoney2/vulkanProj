#pragma once
/*
    VKBootstrap header
*/
#include <VkBootstrap.h>

const int MAX_FRAMES_IN_FLIGHT = 2;

typedef struct vkCtl {
    vkb::Instance  vkbInst;    /* vulkan instance */
    vkb::Device    vkbDevice;  /* vulkan physical device */
    VkQueue        gfxQueue;   /* vulkan graphics queue */
    vkb::Swapchain swapchain;  /* vulkan swapchain */
    VkSurfaceKHR   surface;    /* vulkan surface */
    GlfwInterface* glfw;      /* glfw window interface */
} vkCtl_t;

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