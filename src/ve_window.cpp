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
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	_window = glfwCreateWindow(_width, _height, _window_name.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(_window, this);
	glfwSetFramebufferSizeCallback(_window, frame_buffer_resize_callback);
}

void ve_window::frame_buffer_resize_callback(GLFWwindow* window, int width, int height) {
	auto temp_window = reinterpret_cast<ve_window*>(glfwGetWindowUserPointer(window));
	temp_window->_frame_buffer_resized = true;
	temp_window->_width = width;
	temp_window->_height = height;
}
