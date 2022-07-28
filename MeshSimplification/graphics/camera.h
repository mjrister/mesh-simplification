#pragma once

#include <algorithm>
#include <cmath>
#include <limits>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace qem {

	class Camera {

	public:
		Camera(const glm::vec3& target, const float radius, const float theta, const float phi) noexcept
			: target{target}, radius_{radius}, theta_{theta}, phi_{phi}  {}

		void RotateBy(const float theta, const float phi) {
			theta_ = std::fmod(theta_ + theta, kTwoPi);
			phi_ = std::clamp(phi_ + phi, kPhiMin, kPhiMax);
		}

		[[nodiscard]] glm::mat4 GetViewTransform() const {
			const auto cos_phi = std::cos(phi_);
			const glm::vec3 look_from{
				radius_ * std::sin(theta_) * cos_phi,
				radius_ * std::sin(phi_),
				radius_ * std::cos(theta_) * cos_phi
			};
			return glm::lookAt(look_from + target, target, kWorldUp);
		}

	private:
		static constexpr auto kTwoPi = glm::two_pi<float>();
		static constexpr auto kEpsilon = std::numeric_limits<float>::epsilon();
		static constexpr auto kPhiMin = -glm::half_pi<float>() + kEpsilon;
		static constexpr auto kPhiMax = glm::half_pi<float>() - kEpsilon;
		static constexpr glm::vec3 kWorldUp{0.f, 1.f, 0.f};
		glm::vec3 target;
		float radius_, theta_, phi_;
	};
}
