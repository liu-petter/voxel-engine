#pragma once

#include "ve_device.hpp"
#include "ve_model.hpp"

#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <cassert>

struct pipeline_config_info {
	pipeline_config_info(const pipeline_config_info&) = delete;
	pipeline_config_info& operator=(const pipeline_config_info&) = delete;

	VkPipelineViewportStateCreateInfo viewport_info;
	VkPipelineInputAssemblyStateCreateInfo input_assembly_info;
	VkPipelineRasterizationStateCreateInfo rasterization_info;
	VkPipelineMultisampleStateCreateInfo multisample_info;
	VkPipelineColorBlendAttachmentState color_blend_attachment;
	VkPipelineColorBlendStateCreateInfo color_blend_info;
	VkPipelineDepthStencilStateCreateInfo depth_stencil_info;
	std::vector<VkDynamicState> dynamic_state_enables;
	VkPipelineDynamicStateCreateInfo dynamic_state_info;
	VkPipelineLayout pipeline_layout = nullptr;
	VkRenderPass render_pass = nullptr;
	uint32_t subpass = 0;
};

class ve_pipeline {
public:
	ve_pipeline(
		ve_device &device, 
		const std::string& vert_file_path, 
		const std::string& frag_file_path, 
		const pipeline_config_info &config_info);
	~ve_pipeline();
	ve_pipeline(const ve_pipeline&) = delete;
	ve_pipeline& operator=(const ve_pipeline&) = delete;

	void bind(VkCommandBuffer command_buffer);

	static void default_pipeline_config_info(pipeline_config_info& config_info);
private:
	static std::vector<char> read_file(const std::string& file_path);

	void create_graphics_pipeline(
		const std::string& vert_file_path, 
		const std::string& frag_file_path,
		const pipeline_config_info &config_info);
	
	void create_shader_module(const std::vector<char>& code, VkShaderModule* shader_module);

	ve_device& _ve_device;
	VkPipeline _graphics_pipeline;
	VkShaderModule _frag_shader_module;
	VkShaderModule _vert_shader_module;
};