#include <chrono>
#include <cstdlib>
#include <exception>
#include <format>
#include <iostream>
#include <utility>

#include "graphics/scene.h"
#include "graphics/window.h"

namespace {

class DeltaTime {
  using Clock = std::chrono::steady_clock;
  using Duration = std::chrono::duration<float>;
  using TimePoint = std::chrono::time_point<Clock, Duration>;

public:
  DeltaTime() noexcept : current_time_{Clock::now()}, previous_time_{current_time_} {}

  [[nodiscard]] Duration::rep get() const noexcept { return delta_time_.count(); }

  void Update() noexcept {
    delta_time_ = current_time_ - previous_time_;
    previous_time_ = current_time_;
    current_time_ = Clock::now();
  }

private:
  TimePoint current_time_, previous_time_;
  Duration delta_time_{};
};

}  // namespace

int main() {
  try {
    constexpr auto* kProjectTitle = "Mesh Simplification";
    constexpr auto kWindowDimensions = std::make_pair(1920, 1080);
    constexpr auto kOpenGlVersion = std::make_pair(4, 6);
    qem::Window window{kProjectTitle, kWindowDimensions, kOpenGlVersion};
    qem::Scene scene{&window};

    for (DeltaTime delta_time; !window.IsClosed();) {
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
