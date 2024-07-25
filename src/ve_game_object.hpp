#pragma once

#include "ve_model.hpp"

#include <memory>

struct transform_2d_component {
	glm::vec2 translation{.0f, .0f};
	glm::vec2 scale{1.f, 1.f};
	float rotation = .0f;

	glm::mat2 mat2() {
		const float s = glm::sin(rotation);
		const float c = glm::cos(rotation);

		glm::mat2 rotation_mat = { {c,s},{-s,c} };

		glm::mat2 scale_mat{ {scale.x, .0f}, {.0f, scale.y} };
		return rotation_mat * scale_mat;
	};
};

class ve_game_object {
public:
	using id_t = unsigned int;

	static ve_game_object create_game_object() {
		static id_t current_id = 0;
		return ve_game_object{ current_id++ };
	}

	ve_game_object(const ve_game_object&) = delete;
	ve_game_object& operator=(const ve_game_object&) = delete;
	ve_game_object(ve_game_object&&) = default;
	ve_game_object& operator=(ve_game_object&&) = default;

	id_t get_id() const { return _id; }

	std::shared_ptr<ve_model> _model{};
	glm::vec3 _color{};
	transform_2d_component _transform_2d{};

private:
	ve_game_object(id_t obj_id) : _id{ obj_id } {};
	id_t _id;
};