#pragma once

#include <chrono>

namespace gfx {

/** \brief A utility to calculate the time between frames. */
class DeltaTime {
  using Clock = std::chrono::steady_clock;
  using Duration = std::chrono::duration<float>;
  using TimePoint = std::chrono::time_point<Clock, Duration>;

public:
  /**
   * \brief Gets the current delta time.
   * \return The time in float seconds since <tt>DeltaTime::Update</tt> was called.
   */
  [[nodiscard]] Duration::rep get() const noexcept { return delta_time_.count(); }

  /** \brief Calculates the current delta time. This should for each frame in the main render loop. */
  void Update() noexcept {
    delta_time_ = current_time_ - previous_time_;
    previous_time_ = current_time_;
    current_time_ = Clock::now();
  }

private:
  TimePoint current_time_ = Clock::now();
  TimePoint previous_time_ = current_time_;
  Duration delta_time_{};
};

}  // namespace gfx
