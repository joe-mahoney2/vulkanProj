#include "VkEngine.h"
#include "GlfwInterface.h"

int main() {
	VkEngine engine;

	while (!glfwWindowShouldClose(engine.vkCtl.glfw->window)) {
		glfwPollEvents();
	}
	return 0;
}