#pragma once 
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <array>
#include <chrono>

/*
    Module header containers / module
    using structs/defs
*/
#include "GLMDefs.h"
#include "VKBDefs.h"
#include "STBDefs.h"
#include "VKRenderData.h"

/*
    Engine defined headers
*/
#include "globalDefs.h"
#include "YourMom.h"
#include "GlfwInterface.h"

class VkRender {
public:
    VkRender();
    ~VkRender();

    int initGlfw();
    int initVulkanBootstrap();
    int initVulkanRenderer();
    
    int initVertexBuffers();
    int initQueue();
    int initRenderPass();

private:
    vulkan_ctl_t vulkan;
    RenderData_t render_data;
protected:
};