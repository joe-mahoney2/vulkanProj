cmake_minimum_required(VERSION 3.26.0)
project(vulkan)

#add_subdirectory <- may use this later (compartmentalize stuffs)

#compile shader code
execute_process(COMMAND bash "./shaders/rootBuild.sh")

#INCLUDE PATHS
include_directories(../include)
include_directories(../lib/include)
include_directories(../../modules/include)
include_directories(../../modules/GLFW)
include_directories($ENV{VK_SDK_PATH}/include)


#add options for each render file 
#add_executable(test triangle.cpp)
add_executable(test main.cpp)

#generate defines header
file(WRITE ../include/globalDefs.h "#define SHADER_PATH \"${CMAKE_SOURCE_DIR}/shaders\"\n")
file(APPEND ../include/globalDefs.h "#define SUCCESS 1\n")
file(APPEND ../include/globalDefs.h "#define FAIL 0\n")

target_link_directories(test PUBLIC ../lib)
target_link_directories(test PUBLIC ../../modules/lib)
target_link_directories(test PUBLIC $ENV{VK_SDK_PATH}/Lib)

if(EXISTS ./../lib/VkEngine.lib)
#no-op
else()
    #build VkEngine lib if it's not built
    execute_process(COMMAND bash "../lib/src/build.sh")
endif()

target_link_libraries(test 
    glfw3
    vk-bootstrap
    vulkan-1
    VkEngine
)