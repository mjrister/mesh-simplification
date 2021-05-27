#pragma once

#include <functional>
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

		friend bool operator==(const Vertex& lhs, const Vertex& rhs) {
			return lhs.id_ == rhs.id_ && lhs.position_ == rhs.position_ && lhs.normal_ == rhs.normal_;
		}

		friend bool operator!=(const Vertex& lhs, const Vertex& rhs) {
			return !(lhs == rhs);
		}

		friend std::size_t hash_value(const Vertex& vertex) {
			std::size_t seed = 0x0EAC3880;
			seed ^= (seed << 6) + (seed >> 2) + 0x7CB4C352 + std::hash<decltype(vertex.id_)>{}(vertex.id_);
			seed ^= (seed << 6) + (seed >> 2) + 0x272DC32C + std::hash<decltype(vertex.position_)>{}(vertex.position_);
			seed ^= (seed << 6) + (seed >> 2) + 0x0DE14DA2 + std::hash<decltype(vertex.normal_)>{}(vertex.normal_);
			return seed;
		}

		friend std::ostream& operator<<(std::ostream& os, const Vertex& vertex) {
			return os << vertex.id_;
		}

	private:
		std::uint32_t id_;
		glm::vec3 position_, normal_;
	};
}