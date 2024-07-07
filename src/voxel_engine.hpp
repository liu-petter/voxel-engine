#pragma once

#include "ve_window.hpp"
#include "ve_pipeline.hpp"
#include "ve_device.hpp"
#include "ve_swap_chain.hpp"

#include <memory>
#include <vector>
#include <stdexcept>
#include <array>

class voxel_engine {
public:
	static constexpr int WIDTH = 1200;
	static constexpr int HEIGHT = 750;

	voxel_engine();
	~voxel_engine();

	voxel_engine(const voxel_engine&) = delete;
	voxel_engine& operator=(const voxel_engine&) = delete;

	void run_app();
private:
	void create_pipeline_layout();
	void create_pipeline();
	void create_command_buffers();
	void draw_frame();

	ve_window _window{ WIDTH, HEIGHT, "Voxel Engine" };
	ve_device _device{ _window };
	ve_swap_chain _swap_chain{ _device, _window.get_extent() };
	std::unique_ptr<ve_pipeline> _pipeline;
	VkPipelineLayout _pipeline_layout;
	std::vector<VkCommandBuffer> _command_buffers;
};