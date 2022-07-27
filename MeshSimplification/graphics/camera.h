#pragma once

#include <algorithm>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace qem {

	class Camera {

	public:
		Camera(const float radius, const float theta, const float phi, const glm::vec3& pivot) noexcept
			: radius_{radius}, theta_{theta}, phi_{phi}, pivot_{pivot} {}

		void RotateBy(const float alpha, const float beta) noexcept {
			constexpr auto kEpsilon = std::numeric_limits<float>::epsilon();
			constexpr auto kPhiMin = -glm::half_pi<float>() + kEpsilon;
			constexpr auto kPhiMax = glm::half_pi<float>() - kEpsilon;
			theta_ = std::fmodf(theta_ + alpha, glm::two_pi<float>());
			phi_ = std::clamp(phi_ + beta, kPhiMin, kPhiMax);
		}

		[[nodiscard]] glm::mat4 GetViewTransform() const {
			const glm::vec3 look_from{
				radius_ * std::cos(theta_) * std::cos(-phi_),
				radius_ * std::sin(-phi_),
				radius_ * std::sin(theta_) * std::cos(-phi_)
			};
			constexpr glm::vec3 kWorldUp{0.f, 1.f, 0.f};
			return glm::lookAt(look_from + pivot_, pivot_, kWorldUp);
		}

	private:
		float radius_, theta_, phi_;
		glm::vec3 pivot_;
	};
}
