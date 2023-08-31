cmake_minimum_required(VERSION 3.26.0)
set(CMAKE_OSX_ARCHITECTURES "arm64") # set architecture to arm64 (apple m1)
project(VkEngine)

set(CMAKE_CXX_STANDARD 11) # set compiler standard to c++11

#INCLUDE PATHS
include_directories(../include)
include_directories(../../include)
include_directories(../../../modules/include)
include_directories(../../../modules/GLFW)
include_directories($ENV{VK_SDK_PATH}/include)
include_directories(/Users/cardo/VulkanSDK/1.3.250.1/macOS/include) #cardos vulkan sdk

add_library(VkEngine STATIC 
    VkEngine.cpp 
    GlfwInterface.cpp
    YourMom.cpp
)

target_link_directories(VkEngine PUBLIC ../../../modules/lib)
target_link_directories(VkEngine PUBLIC $ENV{VK_SDK_PATH}/Lib)
target_link_directories(VkEngine PUBLIC /Users/cardo/VulkanSDK/1.3.250.1/macOS/lib) #cardos vulkan sdk

target_link_libraries(VkEngine 
    glfw3
    vk-bootstrap
    vulkan-1
)

install(TARGETS VkEngine DESTINATION "${CMAKE_SOURCE_DIR}/../")