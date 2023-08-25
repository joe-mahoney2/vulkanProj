#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include <VkBootstrap.h>

#include "globalDefs.h"
#include "GlfwInterface.h"

typedef struct {
    vkb::Instance  vkbInst;    /* vulkan instance */
    vkb::Device    vkbDevice;  /* vulkan physical device */
    VkQueue        gfxQueue;   /* vulkan graphics queue */
    vkb::Swapchain swapchain;  /* vulkan swapchain */
    VkSurfaceKHR   surface;    /* vulkan surface */
    GlfwInterface  *glfw;      /* glfw window interface */
} vkCtl_t;


class VkEngine {
    public:
        VkEngine();
        ~VkEngine();
        int initGlfw();
        int initVulkan();

        vkCtl_t vkCtl;
    private:
};