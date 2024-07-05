#pragma once

#include "VkWindow.hpp"

class VoxelEngine {
public:
	static constexpr int WIDTH = 1200;
	static constexpr int HEIGHT = 750;

	void runApp();
private:
	VkWindow vkWindow{ WIDTH, HEIGHT, "Voxel Engine" };
};