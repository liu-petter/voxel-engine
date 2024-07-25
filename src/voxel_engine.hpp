#pragma once

#include "ve_window.hpp"
#include "ve_pipeline.hpp"
#include "ve_device.hpp"
#include "ve_swap_chain.hpp"
#include "ve_model.hpp"
#include "ve_game_object.hpp"

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
	void load_game_objects();
	void create_pipeline_layout();
	void create_pipeline();
	void create_command_buffers();
	void free_command_buffers();
	void draw_frame();
	void recreate_swap_chain();
	void record_command_buffer(int image_index);
	void render_game_objects(VkCommandBuffer command_buffer);

	ve_window _window{ WIDTH, HEIGHT, "Voxel Engine" };
	ve_device _device{ _window };
	std::unique_ptr<ve_swap_chain> _swap_chain;
	std::unique_ptr<ve_pipeline> _pipeline;
	VkPipelineLayout _pipeline_layout;
	std::vector<VkCommandBuffer> _command_buffers;
	std::vector<ve_game_object> _game_objects;
};