#pragma once

#include <chrono>

namespace gfx {

/** @brief A utility for measuring the time between frames. */
class DeltaTime {
public:
  /** @brief A type alias for @c std::chrono::duration that represents time as float seconds. */
  using FloatSeconds = std::chrono::duration<float>;

  /** @brief The amount of time elapsed since the previous frame in seconds. */
  [[nodiscard]] FloatSeconds::rep get() const noexcept { return delta_time_; }

  /** @brief Calculates the current delta time. This should for each frame in the main render loop. */
  void Update() noexcept {
    const auto current_time = Clock::now();
    const auto float_seconds = std::chrono::duration_cast<FloatSeconds>(current_time - previous_time_);
    delta_time_ = float_seconds.count();
    previous_time_ = current_time;
  }

private:
  using Clock = std::chrono::steady_clock;
  using TimePoint = std::chrono::time_point<Clock>;

  TimePoint previous_time_ = Clock::now();
  FloatSeconds::rep delta_time_ = 0.0f;
};

}  // namespace gfx
