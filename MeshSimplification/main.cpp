#include <chrono>
#include <cstdlib>
#include <exception>
#include <format>
#include <iostream>
#include <utility>

#include "graphics/scene.h"
#include "graphics/window.h"

int main() {
  try {
    constexpr auto* kProjectTitle = "Mesh Simplification";
    constexpr auto kWindowDimensions = std::make_pair(1280, 960);
    constexpr auto kOpenGlVersion = std::make_pair(4, 6);
    qem::Window window{kProjectTitle, kWindowDimensions, kOpenGlVersion};
    qem::Scene scene{&window};

    auto current_time = std::chrono::steady_clock::now();
    auto previous_time = current_time;

    while (!window.IsClosed()) {
      const auto delta_time = std::chrono::duration<float>(current_time - previous_time).count();
      previous_time = current_time;
      current_time = std::chrono::steady_clock::now();

      window.Update();
      scene.Render(delta_time);
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "An unknown error occurred" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
