#include <cstdlib>
#include <exception>
#include <iostream>
#include <utility>

#include "graphics/delta_time.h"
#include "graphics/scene.h"
#include "graphics/window.h"

int main() {
  try {
    constexpr auto* kProjectTitle = "Mesh Simplification";
    constexpr auto kWindowDimensions = std::make_pair(1920, 1080);
    constexpr auto kOpenGlVersion = std::make_pair(4, 6);
    qem::Window window{kProjectTitle, kWindowDimensions, kOpenGlVersion};
    qem::Scene scene{&window};

    for (qem::DeltaTime delta_time; !window.IsClosed();) {
      delta_time.Update();
      window.Update();
      scene.Render(delta_time.get());
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
