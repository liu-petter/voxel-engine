#include "ve_window.hpp"

ve_window::ve_window(int w, int h, std::string name) : _width{ w }, _height{ h }, _window_name{ name } {	
	init_window();
}

ve_window::~ve_window() {
	glfwDestroyWindow(_window);
	glfwTerminate();
}

void ve_window::create_window_surface(VkInstance instance, VkSurfaceKHR* surface) {
	if (glfwCreateWindowSurface(instance, _window, nullptr, surface) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create window surface");
	}
}

void ve_window::init_window() {
	glfwInit();
	// disable OpenGL context
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	_window = glfwCreateWindow(_width, _height, _window_name.c_str(), nullptr, nullptr);
}