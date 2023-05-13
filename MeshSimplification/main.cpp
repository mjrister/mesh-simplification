#include <cstdlib>
#include <exception>
#include <format>
#include <iostream>
#include <utility>

#include "graphics/frame_timer.h"
#include "graphics/scene.h"
#include "graphics/window.h"

int main() {

	try {
		constexpr auto kWindowDimensions = std::make_pair(1280, 960);
		constexpr auto kOpenGlVersion = std::make_pair(4, 6);
		constexpr auto* kProjectTitle = "Mesh Simplification";
		qem::Window window{kProjectTitle, kWindowDimensions, kOpenGlVersion};
		qem::Scene scene{&window};
		qem::FrameTimer frame_timer;

		for (auto elapsed_time = 0.0f; !window.IsClosed();) {
			frame_timer.Update();
			const auto delta_time = frame_timer.delta_time();

			if (elapsed_time += delta_time; elapsed_time > frame_timer.sample_time()) {
				const auto frames_per_second = static_cast<std::uint32_t>(frame_timer.frames_per_second());
				const auto window_title = std::format("{} - Frames per second: {}", kProjectTitle, frames_per_second);
				window.SetTitle(window_title.c_str());
				elapsed_time = 0.0f;
			}

			window.Update();
			scene.Render(delta_time);
		}
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
