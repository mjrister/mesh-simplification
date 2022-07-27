#pragma once

#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace qem {

	class Camera {

	public:
		explicit Camera(const float radius, const float theta = 0.f, const float phi = 0.f) noexcept
			: radius_{radius}, theta_{theta}, phi_{phi} {
		}

		void Rotate(const float theta, const float phi) noexcept {
			theta_ += theta;
			phi_ += phi;
		}

		[[nodiscard]] glm::mat4 GetViewTransform() const {
			const glm::vec3 look_from{
				radius_ * std::cos(theta_) * std::cos(-phi_),
				radius_ * std::sin(-phi_),
				radius_ * std::sin(theta_) * std::cos(-phi_)
			};
			return lookAt(look_from, kLookAt, kWorldUp);
		}

	private:
		static constexpr glm::vec3 kLookAt{0.f};
		static constexpr glm::vec3 kWorldUp{0.f, 1.f, 0.f};
		float radius_, theta_, phi_;
	};
}
