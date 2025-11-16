#include <cstdlib>
#include <exception>
#include <iostream>
#include <utility>

#include "app.h"  // NOLINT(build/include_subdir)

int main() {  // NOLINT(bugprone-exception-escape): exceptions are not enabled for std::cerr
  try {
    static constexpr auto* kProjectTitle = "Mesh Simplification";
    static constexpr auto kWindowSize = std::make_pair(1920, 1080);
    static constexpr auto kOpenGlVersion = std::make_pair(4, 1);
    app::Run(kProjectTitle, kWindowSize, kOpenGlVersion);
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "An unknown error occurred\n";
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
