#pragma once

#include <ostream>

#include <glm/vec3.hpp>
#include <glm/gtx/hash.hpp>

namespace geometry {

	class Vertex {

	public:
		Vertex(const std::uint32_t id, const glm::vec3& position, const glm::vec3& normal)
			: id_{id}, position_{position}, normal_{normal} {}

		[[nodiscard]] auto Id() const { return id_; }
		[[nodiscard]] const auto& Position() const { return position_; }
		[[nodiscard]] const auto& Normal() const { return normal_; }

	private:
		const std::uint32_t id_;
		const glm::vec3 position_, normal_;
	};
}
