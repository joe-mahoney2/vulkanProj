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

    // setup all GLFW window callbacks
    glfwSetCursorPosCallback(window, (GLFWcursorposfun) cursorPosition);
    glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun) mouseButtonPress);

    return status;
}

void GlfwInterface::cursorPosition(GLFWwindow* win, double x, double y) {
    winInput.cursorX = x;
    winInput.cursorY = y;
    // std::cout << "cursor callback x::y -> " <<  winInput.cursorX << "::" << winInput.cursorY << std::endl;
}


void GlfwInterface::mouseButtonPress(GLFWwindow* window, int button, int action, int mods) {
    switch (button)
    {
        // determine button states
        case GLFW_MOUSE_BUTTON_LEFT:
            buttonHandle(action, &winInput.leftMb);
            // std::cout << "mouse LMB state -> " << winInput.leftMb <<std::endl;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            buttonHandle(action, &winInput.rightMb);
            // std::cout << "mouse RMB state -> " << winInput.rightMb <<std::endl;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            buttonHandle(action, &winInput.midMb);
            // std::cout << "mouse MMB state -> " << winInput.midMb <<std::endl;
            break;
        default:
            break;
    }
}

void GlfwInterface::buttonHandle(int action, bool *inputButton) {
    switch (action)
    {
        case GLFW_PRESS:
            *inputButton = true;
            break;
        case GLFW_RELEASE:
            *inputButton = false;
            break;
        case GLFW_REPEAT:
            *inputButton = true;
            break;
        default:
            std::cout << "GlfwInterface::buttonHandle error!" << std::endl;
        break;
    };
}

GlfwInterface::~GlfwInterface() {}