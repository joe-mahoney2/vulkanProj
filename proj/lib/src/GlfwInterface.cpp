#include "GlfwInterface.h"

GlfwInterface::GlfwInterface() { 
    initWindow();
} 

int GlfwInterface::initWindow() {
    int status = SUCCESS;
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    if ((window = glfwCreateWindow(512, 512, "glfwWindow", NULL, NULL)) == NULL) {
        std::cout << "GlfwInterface::initWindow Failure to create Window" << std::endl;
        status = FAIL;
    }

    if (glfwSetCursorPosCallback(window, (GLFWcursorposfun) cursorPosition) == NULL) {
        std::cout << "GlfwInterface::initWindow Failure glfwSetCursorPosCallback" << std::endl;
        status = FAIL;
    }

    return status;
}

void GlfwInterface::cursorPosition(GLFWwindow* win, double x, double y) {
    winInput.cursorX = x;
    winInput.cursorY = y;
    std::cout << "cursor callback x::y -> " <<  winInput.cursorX << "::" << winInput.cursorY << std::endl;
}

GlfwInterface::~GlfwInterface() {}