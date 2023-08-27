#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

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

typedef struct RenderData {
	VkQueue gfxQueue;                         /**/
	VkQueue present_queue;                          /**/
	std::vector<VkImage> scImages;                  /**/
	std::vector<VkImageView> scImageViews;          /**/
	std::vector<VkFramebuffer> frameBuffers;        /**/
	VkRenderPass renderPass;                        /* rendering stuff in passes kinda*/
	VkPipelineLayout pipeLayout;                    /**/
	VkPipeline gfxPipe;                             /**/
	VkCommandPool cmdPool;                          /**/
	std::vector<VkCommandBuffer> cmdBuffers;        /**/
	std::vector<VkSemaphore> available_semaphores;  /**/
	std::vector<VkSemaphore> finished_semaphore;    /**/
	std::vector<VkFence> in_flight_fences;          /**/
	std::vector<VkFence> image_in_flight;           /**/
	size_t current_frame = 0;                       /* current frame*/
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

        int recreateSwapChain();
        int drawFrame();

        VkShaderModule createShaderModule(const std::vector<char>&);

        vkCtl_t vkCtl; 
        RenderData_t vkRen;
    private:
};