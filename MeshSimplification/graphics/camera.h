#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace qem {

	class Camera {

	public:
		Camera() = default;

		[[nodiscard]] glm::mat4 LookAt() const {
			return lookAt(look_from_, look_at_, world_up_);
		}

	private:
		glm::vec3 look_from_{0.f, .4f, 2.f};
		glm::vec3 look_at_{0.f};
		glm::vec3 world_up_{0.f, 1.f, 0.f};
	};
}
