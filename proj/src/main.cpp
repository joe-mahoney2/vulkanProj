#include "VkEngine.h"
#include "GlfwInterface.h"

//define GlfwInterface interface static structure
glfwInput_t GlfwInterface::winInput;

int main() {
	VkEngine engine;

	while (!glfwWindowShouldClose(engine.vkCtl.glfw->window)) {
		glfwPollEvents();
		if (engine.drawFrame() == FAIL)
			break;
	}

	delete &engine;
	return 0;
}