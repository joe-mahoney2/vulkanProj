cmake_minimum_required(VERSION 3.26.0)
project(vulkan)

# check system processor for archirecture and launch accordingly
if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
    set(CMAKE_OSX_ARCHITECTURES "arm64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    set(CMAKE_OSX_ARCHITECTURES "x86_64")
else()
    message(FATAL_ERROR "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(CMAKE_CXX_STANDARD 11) #set c++ standard to c++11 (vulkan uses c++11)

#add_subdirectory <- may use this later (compartmentalize stuffs)

#compile shader code
execute_process(COMMAND bash "./shaders/rootBuild.sh")

#INCLUDE PATHS
include_directories(../include)
include_directories(../lib/include)
include_directories(../../modules/include)
include_directories(../../modules/GLFW)
include_directories($ENV{VK_SDK_PATH}/include)

# add_compile_options(-Wno-c++11-extensions) #ignore c++11 extensions warning


#add options for each render file 
add_executable(test main.cpp)

#generate defines header
file(WRITE ../include/globalDefs.h "#define SHADER_PATH \"${CMAKE_SOURCE_DIR}/shaders\"\n")
file(APPEND ../include/globalDefs.h "#define SUCCESS 1\n")
file(APPEND ../include/globalDefs.h "#define FAIL 0\n")

target_link_directories(test PUBLIC ../lib)
target_link_directories(test PUBLIC ../../modules/lib)
target_link_directories(test PUBLIC $ENV{VK_SDK_PATH}/lib)

find_package(glfw3 REQUIRED)
target_link_libraries(test glfw)

if(EXISTS ./../lib/VkEngine.lib)
#no-op
else()
    #build VkEngine lib if it's not built
    execute_process(COMMAND bash "../lib/src/build.sh")
endif()

if (APPLE)
  set(VK_LIB_NAME "vulkan")
else()
  set(VK_LIB_NAME "vulkan-1")
endif()

target_link_libraries(test 
    glfw3
    vk-bootstrap
    ${VK_LIB_NAME}
    VkEngine
)