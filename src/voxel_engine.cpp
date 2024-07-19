#include "voxel_engine.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

struct simple_push_constant_data {
	glm::vec2 offset;
	alignas(16) glm::vec3 color;
};

voxel_engine::voxel_engine() {
	load_models();
	create_pipeline_layout();
	recreate_swap_chain();
	create_command_buffers();
}

voxel_engine::~voxel_engine() {
	vkDestroyPipelineLayout(_device.device(), _pipeline_layout, nullptr);
}

void voxel_engine::run_app() {
	while (!_window.should_close()) {
		glfwPollEvents();
		draw_frame();
	}
	vkDeviceWaitIdle(_device.device());
}

void voxel_engine::load_models() {
	std::vector<ve_model::vertex> vertices{
		{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
	};
	_model = std::make_unique<ve_model>(_device, vertices);
}

void voxel_engine::create_pipeline_layout() {
	VkPushConstantRange push_constant_range{};
	push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	push_constant_range.offset = 0;
	push_constant_range.size = sizeof(simple_push_constant_data);

	VkPipelineLayoutCreateInfo pipeline_layout_info{};
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_info.setLayoutCount = 0;
	pipeline_layout_info.pSetLayouts = nullptr;
	pipeline_layout_info.pushConstantRangeCount = 1;
	pipeline_layout_info.pPushConstantRanges = &push_constant_range;
	if (vkCreatePipelineLayout(_device.device(), &pipeline_layout_info, nullptr, &_pipeline_layout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create pipeline layout");
	}
}

void voxel_engine::create_pipeline() {
	assert(_swap_chain != nullptr && "Cannot create pipeline before swap chain");
	assert(_pipeline_layout != nullptr && "Cannot create pipeline before pipeline layout");

	pipeline_config_info pipeline_config{};
	ve_pipeline::default_pipeline_config_info(pipeline_config);
	pipeline_config.render_pass = _swap_chain->getRenderPass();
	pipeline_config.pipeline_layout = _pipeline_layout;
	_pipeline = std::make_unique<ve_pipeline>(_device, ".\\shaders\\simple_shader.vert.spv", ".\\shaders\\simple_shader.frag.spv", pipeline_config);
}

void voxel_engine::recreate_swap_chain() {
	auto extent = _window.get_extent();
	while (extent.width == 0 || extent.height == 0) {
		extent = _window.get_extent();
		glfwWaitEvents();
	}
	vkDeviceWaitIdle(_device.device());

	if (_swap_chain == nullptr) {
		_swap_chain = std::make_unique<ve_swap_chain>(_device, extent);
	}
	else {
		_swap_chain = std::make_unique<ve_swap_chain>(_device, extent, std::move(_swap_chain)); 
		if (_swap_chain->imageCount() != _command_buffers.size()) {
			free_command_buffers();
			create_command_buffers();
		}
	}
	create_pipeline();
}

void voxel_engine::create_command_buffers() {
	_command_buffers.resize(_swap_chain->imageCount());

	VkCommandBufferAllocateInfo alloc_info{};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandPool = _device.getCommandPool();
	alloc_info.commandBufferCount = static_cast<uint32_t> (_command_buffers.size());

	if (vkAllocateCommandBuffers(_device.device(), &alloc_info, _command_buffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate command buffers");
	}
} 

void voxel_engine::free_command_buffers() {
	vkFreeCommandBuffers(
		_device.device(),
		_device.getCommandPool(),
		static_cast<uint32_t>(_command_buffers.size()),
		_command_buffers.data());
	_command_buffers.clear();
}


void voxel_engine::record_command_buffer(int image_index) {
	static int frame = 0;
	frame = (frame + 1) % 100;

	VkCommandBufferBeginInfo begin_info{};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(_command_buffers[image_index], &begin_info) != VK_SUCCESS) {
		throw std::runtime_error("Failed to begin recording command buffer");
	}

	VkRenderPassBeginInfo render_pass_info{};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_info.renderPass = _swap_chain->getRenderPass();
	render_pass_info.framebuffer = _swap_chain->getFrameBuffer(image_index);

	render_pass_info.renderArea.offset = { 0,0 };
	render_pass_info.renderArea.extent = _swap_chain->getSwapChainExtent();

	std::array<VkClearValue, 2> clear_values{};
	clear_values[0].color = { 0.01f, 0.01f, 0.01f,  1.0f };
	clear_values[1].depthStencil = { 1.0f, 0 };
	render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
	render_pass_info.pClearValues = clear_values.data();

	vkCmdBeginRenderPass(_command_buffers[image_index], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(_swap_chain->getSwapChainExtent().width);
	viewport.height = static_cast<float>(_swap_chain->getSwapChainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	VkRect2D scissor{ {0,0}, _swap_chain->getSwapChainExtent() };
	vkCmdSetViewport(_command_buffers[image_index], 0, 1, &viewport);
	vkCmdSetScissor(_command_buffers[image_index], 0, 1, &scissor);

	_pipeline->bind(_command_buffers[image_index]);
	_model->bind(_command_buffers[image_index]);

	for (int j = 0; j < 4; j++) {
		simple_push_constant_data push{};
		push.offset = { -0.5f + frame * 0.02f, -0.4f + j * 0.25f };
		push.color = { 0.0f, 0.0f, 0.2f + 0.2f * j };

		vkCmdPushConstants(
			_command_buffers[image_index], 
			_pipeline_layout, 
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
			0, 
			sizeof(simple_push_constant_data), 
			&push
		);

		_model->draw(_command_buffers[image_index]);
	}

	vkCmdEndRenderPass(_command_buffers[image_index]);
	if (vkEndCommandBuffer(_command_buffers[image_index]) != VK_SUCCESS) {
		throw std::runtime_error("Failed to record command buffer");
	}
}

void voxel_engine::draw_frame() {
	uint32_t image_index;
	auto result = _swap_chain->acquireNextImage(&image_index);

	// resets swap chain if window is resized
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreate_swap_chain();
		return;
	}

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("Failed to acquire swap chain image");
	}

	record_command_buffer(image_index);
	result = _swap_chain->submitCommandBuffers(&_command_buffers[image_index], &image_index);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _window.was_window_resized()) {
		_window.reset_window_resized_flag();
		recreate_swap_chain();
		return;
	}

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to present swap chain image");
	}
}