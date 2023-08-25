#include "GlfwInterface.h"

GlfwInterface::GlfwInterface() { 
    initWindow();
} 

int GlfwInterface::initWindow() {
    int status = SUCCESS;
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    if ((window = glfwCreateWindow(1024, 1024, "glfwWindow", NULL, NULL)) == NULL) {
        std::cout << "GlfwInterface::initWindow Failure to create Window" << std::endl;
        status = FAIL;
    }
    return status;
}

GlfwInterface::~GlfwInterface() {}