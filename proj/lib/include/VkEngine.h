#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <array>
#include <chrono>

#define STB_IMAGE_IMPLEMENTATION
#define STBIDEF inline // this took me 30 mfin mins
#include <stb/stb_image.h> 

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <VkBootstrap.h>

#include "globalDefs.h"
#include "YourMom.h"
#include "GlfwInterface.h"


const int MAX_FRAMES_IN_FLIGHT = 2;

typedef struct vkCtl {
    vkb::Instance  vkbInst;    /* vulkan instance */
    vkb::Device    vkbDevice;  /* vulkan physical device */
    VkQueue        gfxQueue;   /* vulkan graphics queue */
    vkb::Swapchain swapchain;  /* vulkan swapchain */
    VkSurfaceKHR   surface;    /* vulkan surface */
    GlfwInterface  *glfw;      /* glfw window interface */
} vkCtl_t;

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;
};

// const std::vector<Vertex> vertices = {
//     {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
//     {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
//     {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
// };


const std::vector<uint16_t> indices {
    0, 1, 2, 2, 3, 0
};

const std::vector<Vertex> vertices {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};


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
    std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions;
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
	size_t current_frame = 0;                      /* current frame*/
} RenderData_t;

class VkEngine {
    public:
        VkEngine();
        ~VkEngine();
        // initializer
        int initGlfw();
        int initVulkan();
        int initSwapChain();
        int initRenderData();
        int initGfxPipeline();
        int initFrameBuffer();
        int initCmdBuffer();
        int initCmdPool();
        int initSyncObjects();

        int initBindingDescription();
        int initVertexBuffers();

        int copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        int createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, 
            VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        int createVertexBuffer();
        int createIndexBuffers();
        
        int initDescriptorSetLayout();
        int initUniformBuffers();

        int initDescriptorPool();
        int initDescriptorSets();

        uint32_t findVkMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        int recreateSwapChain();
        int updateUniformBuffer(uint32_t current_image);
        int drawFrame();

        VkShaderModule createShaderModule(const std::vector<char>&);

        void createTextureImage();
        void createImage(uint32_t width, uint32_t height, VkFormat format, 
            VkImageTiling tiling, VkImageUsageFlags usage, 
            VkMemoryPropertyFlags properties, VkImage& image, 
            VkDeviceMemory& imageMemory);

        void transitionImageLayout(VkImage image, VkFormat format, 
            VkImageLayout oldLayout, VkImageLayout newLayout);

        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);
        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

        vkCtl_t vkCtl; 
        RenderData_t vkRen;
    private:
};