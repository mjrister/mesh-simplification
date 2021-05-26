#pragma once

#include <glm/vec3.hpp>

namespace geometry {

	class Vertex {

	public:
		Vertex(const std::uint32_t id, const glm::vec3& position, const glm::vec3& normal)
			: id_{id}, position_{position}, normal_{normal} {}

		[[nodiscard]] std::int32_t Id() const { return id_; }
		[[nodiscard]] const glm::vec3& Position() const { return position_; }
		[[nodiscard]] const glm::vec3& Normal() const { return normal_; }

	private:
		std::uint32_t id_;
		glm::vec3 position_, normal_;
	};
}