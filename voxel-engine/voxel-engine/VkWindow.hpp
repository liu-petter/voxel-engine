#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

class VkWindow {
public:
	VkWindow(int w, int h, std::string name);
	~VkWindow();

	bool shouldClose() { return glfwWindowShouldClose(window); };
private:
	const int width;
	const int height;
	std::string windowName;
	GLFWwindow* window;

	void initWindow();
};