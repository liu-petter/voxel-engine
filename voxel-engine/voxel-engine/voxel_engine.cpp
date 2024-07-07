#include "voxel_engine.hpp"

voxel_engine::voxel_engine() {
	create_pipeline_layout();
	create_pipeline();
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

void voxel_engine::create_pipeline_layout() {
	VkPipelineLayoutCreateInfo pipeline_layout_info{};
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_info.setLayoutCount = 0;
	pipeline_layout_info.pSetLayouts = nullptr;
	pipeline_layout_info.pushConstantRangeCount = 0;
	pipeline_layout_info.pPushConstantRanges = nullptr;
	if (vkCreatePipelineLayout(_device.device(), &pipeline_layout_info, nullptr, &_pipeline_layout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create pipeline layout");
	}
}

void voxel_engine::create_pipeline() {
	auto pipeline_config = ve_pipeline::default_pipeline_config_info(_swap_chain.width(), _swap_chain.height());
	pipeline_config.render_pass = _swap_chain.getRenderPass();
	pipeline_config.pipeline_layout = _pipeline_layout;
	_pipeline = std::make_unique<ve_pipeline>(_device, "simple_shader.vert.spv", "simple_shader.frag.spv", pipeline_config);
}

void voxel_engine::create_command_buffers() {
	_command_buffers.resize(_swap_chain.imageCount());

	VkCommandBufferAllocateInfo alloc_info{};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandPool = _device.getCommandPool();
	alloc_info.commandBufferCount = static_cast<uint32_t> (_command_buffers.size());

	if (vkAllocateCommandBuffers(_device.device(), &alloc_info, _command_buffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate command buffers");
	}

	for (int i = 0; i < _command_buffers.size(); i++) {
		VkCommandBufferBeginInfo begin_info{};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(_command_buffers[i], &begin_info) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording command buffer");
		}

		VkRenderPassBeginInfo render_pass_info{};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_info.renderPass = _swap_chain.getRenderPass();
		render_pass_info.framebuffer = _swap_chain.getFrameBuffer(i);

		render_pass_info.renderArea.offset = { 0,0 };
		render_pass_info.renderArea.extent = _swap_chain.getSwapChainExtent();

		std::array<VkClearValue, 2> clear_values{};
		clear_values[0].color = { 0.1f, 0.1f, 0.1f,  1.0f };
		clear_values[1].depthStencil = { 1.0f, 0 };
		render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
		render_pass_info.pClearValues = clear_values.data();

		vkCmdBeginRenderPass(_command_buffers[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

		_pipeline->bind(_command_buffers[i]);
		vkCmdDraw(_command_buffers[i], 3, 1, 0, 0);

		vkCmdEndRenderPass(_command_buffers[i]);
		if (vkEndCommandBuffer(_command_buffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer");
		}
	}
} 

void voxel_engine::draw_frame() {
	uint32_t image_index;
	auto result = _swap_chain.acquireNextImage(&image_index);

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("Failed to acquire swap chain image");
	}

	result = _swap_chain.submitCommandBuffers(&_command_buffers[image_index], &image_index);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to present swap chain image");
	}
}