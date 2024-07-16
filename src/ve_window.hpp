#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <stdexcept>

class ve_window {
public:
	ve_window(int w, int h, std::string name);
	~ve_window();

	// delete copy constructor and copy operator
	// due to using pointer for window
	ve_window(const ve_window&) = delete;
	ve_window& operator=(const ve_window&) = delete;

	bool should_close() { return glfwWindowShouldClose(_window); };
	VkExtent2D get_extent() { return { static_cast<uint32_t>(_width), static_cast<uint32_t>(_height) }; };
	bool was_window_resized() { return _frame_buffer_resized; };
	void reset_window_resized_flag() { _frame_buffer_resized = false; };

	void create_window_surface(VkInstance instance, VkSurfaceKHR* surface);
private:
	int _width;
	int _height;
	bool _frame_buffer_resized = false;

	std::string _window_name;
	GLFWwindow* _window;

	void init_window();
	static void frame_buffer_resize_callback(GLFWwindow* window, int width, int height);
};