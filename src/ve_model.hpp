#pragma once

#include "ve_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vector>

// gives vertex date to pipeline
class ve_model {
public:
	struct vertex {
		glm::vec2 positions;
		glm::vec3 color;
		static std::vector<VkVertexInputBindingDescription> get_binding_descriptions();
		static std::vector<VkVertexInputAttributeDescription> get_attribute_descriptions();
	};

	ve_model(ve_device& device, const std::vector<vertex>& vertices);
	~ve_model();

	ve_model(const ve_model&) = delete;
	ve_model& operator=(const ve_model&) = delete;

	void bind(VkCommandBuffer command_buffer);
	void draw(VkCommandBuffer command_buffer);
private:
	void create_vertex_buffers(const std::vector<vertex>& vertices);

	ve_device &_device;
	VkBuffer _vertex_buffer;
	VkDeviceMemory _vertex_buffer_memory;
	uint32_t _vertex_count;
};