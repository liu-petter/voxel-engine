#include "VoxelEngine.hpp"

void VoxelEngine::runApp() {
	while (!vkWindow.shouldClose()) {
		glfwPollEvents();
	}
}