cmake_minimum_required(VERSION 3.26.0)
project(vulkan)

#add_subdirectory <- may use this later (compartmentalize stuffs)
#also looking for options in showing compiler args on build 

#INCLUDE PATHS
include_directories(../include)
include_directories(../../modules/include)
include_directories(../../modules/GLFW)
include_directories($ENV{VK_SDK_PATH}/include)

add_executable(test main.cpp)

#generate defines header
file(WRITE ../include/globalDefs.h "#define SHADER_PATH \"${CMAKE_SOURCE_DIR}/shaders\"")

target_link_directories(test PUBLIC ../../modules/lib)
target_link_directories(test PUBLIC $ENV{VK_SDK_PATH}/Lib)

target_link_libraries(test 
    glfw3
    vk-bootstrap
    vulkan-1
)

