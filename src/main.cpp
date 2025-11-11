#include <cstdlib>
#include <exception>
#include <iostream>
#include <utility>

#include "graphics/delta_time.h"
#include "graphics/scene.h"
#include "graphics/window.h"

int main() {  // NOLINT(bugprone-exception-escape): exceptions are not enabled for std::cerr
  try {
    static constexpr auto* kProjectTitle = "Mesh Simplification";
    static constexpr auto kWindowSize = std::make_pair(1920, 1080);
    static constexpr auto kOpenGlVersion = std::make_pair(4, 1);
    gfx::Window window{kProjectTitle, kWindowSize, kOpenGlVersion};
    gfx::Scene scene{&window};

    for (gfx::DeltaTime delta_time; !window.IsClosed();) {
      delta_time.Update();
      window.Update();
      scene.Render(delta_time.get());
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "An unknown error occurred\n";
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
