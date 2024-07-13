#include "ve_model.hpp"

#include <cassert>
#include <cstring>

ve_model::ve_model(ve_device& device, const std::vector<vertex>& vertices) : _device{device} {
	create_vertex_buffers(vertices);
}

ve_model::~ve_model() {
	vkDestroyBuffer(_device.device(), _vertex_buffer, nullptr);
	vkFreeMemory(_device.device(), _vertex_buffer_memory, nullptr);
}

void ve_model::create_vertex_buffers(const std::vector<vertex>& vertices) {
	_vertex_count = static_cast<uint32_t>(vertices.size());
	assert(_vertex_count >= 3 && "Vertex count must be at least 3");
	VkDeviceSize buffer_size = sizeof(vertices[0]) * _vertex_count;

	_device.createBuffer(
		buffer_size,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		_vertex_buffer,
		_vertex_buffer_memory);

	void* data;
	vkMapMemory(_device.device(), _vertex_buffer_memory, 0, buffer_size, 0, &data);
	memcpy(data, vertices.data(), static_cast<uint32_t>(buffer_size));
	vkUnmapMemory(_device.device(), _vertex_buffer_memory);
}

void ve_model::bind(VkCommandBuffer command_buffer) {
	VkBuffer buffers[] = { _vertex_buffer };
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers, offsets);
}

void ve_model::draw(VkCommandBuffer command_buffer) {
	vkCmdDraw(command_buffer, _vertex_count, 1, 0, 0);
}

std::vector<VkVertexInputBindingDescription> ve_model::vertex::get_binding_descriptions() {
	std::vector<VkVertexInputBindingDescription> binding_descriptions(1);
	binding_descriptions[0].binding = 0;
	binding_descriptions[0].stride = sizeof(vertex);
	binding_descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return binding_descriptions;
}

std::vector<VkVertexInputAttributeDescription> ve_model::vertex::get_attribute_descriptions() {
	std::vector<VkVertexInputAttributeDescription> attribute_description(2);
	attribute_description[0].binding = 0;
	attribute_description[0].location = 0;
	attribute_description[0].format = VK_FORMAT_R32G32_SFLOAT;
	attribute_description[0].offset = offsetof(vertex, positions);

	attribute_description[1].binding = 0;
	attribute_description[1].location = 1;
	attribute_description[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attribute_description[1].offset = offsetof(vertex, color);
	return attribute_description;
}
