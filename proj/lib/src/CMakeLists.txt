cmake_minimum_required(VERSION 3.26.0)
project(VkEngine)

#INCLUDE PATHS
include_directories(../include)
include_directories(../../include)
include_directories(../../../modules/include)
include_directories(../../../modules/GLFW)
include_directories($ENV{VK_SDK_PATH}/include)

add_library(VkEngine STATIC 
    VkEngine.cpp 
    GlfwInterface.cpp
)

target_link_directories(VkEngine PUBLIC ../../../modules/lib)
target_link_directories(VkEngine PUBLIC $ENV{VK_SDK_PATH}/Lib)

target_link_libraries(VkEngine 
    glfw3
    vk-bootstrap
    vulkan-1
)

install(TARGETS VkEngine DESTINATION "${CMAKE_SOURCE_DIR}/../")