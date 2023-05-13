#pragma once

#include <chrono>

namespace qem {

/** \brief A utility class to capture frame rendering performance characteristics. */
class FrameTimer {
	using Clock = std::chrono::steady_clock;
	using Duration = std::chrono::duration<float>;
	using TimePoint = std::chrono::time_point<Clock, Duration>;

public:
	/**
	 * \brief Initializes a frame timer.
	 * \param sample_time The sample interval in seconds to measure frames per second.
	 */
	explicit FrameTimer(const float sample_time = 1.0f) noexcept
		: sample_time_{sample_time} {}

	/** \brief Gets the time between two subsequent frames. */
	[[nodiscard]] Duration::rep delta_time() const noexcept { return delta_time_; }

	/** \brief Gets the number of frames rendered per second. */
	[[nodiscard]] Duration::rep frames_per_second() const noexcept { return frames_per_second_; }

	/** \brief Gets the sample interval time in seconds used to measure frames per second. */
	[[nodiscard]] Duration::rep sample_time() const noexcept { return sample_time_; }

	/** \brief Updates frame rendering performance characteristics. */
	void Update() noexcept {
		const TimePoint current_time = Clock::now();
		UpdateDeltaTime(current_time);
		UpdateFramesPerSecond(current_time);
	}

private:
	/**
	 * \brief Updates the time between two frames.
	 * \param current_time The start time of the current frame.
	 */
	void UpdateDeltaTime(const TimePoint& current_time) noexcept {
		delta_time_ = (current_time - previous_time_).count();
		previous_time_ = current_time;
	}

	/**
	 * \brief Updates the frames per second.
	 * \param current_time The start time of the current frame.
	 */
	void UpdateFramesPerSecond(const TimePoint& current_time) noexcept {
		const auto elapsed_time = (current_time - sample_start_time_).count();
		frames_per_second_ = static_cast<Duration::rep>(++frame_count_) / elapsed_time;

		if (elapsed_time > sample_time_) {
			frame_count_ = 0;
			sample_start_time_ = current_time;
		}
	}

	TimePoint previous_time_ = Clock::now();
	TimePoint sample_start_time_ = Clock::now();
	Duration::rep delta_time_{};
	Duration::rep sample_time_;
	Duration::rep frames_per_second_{};
	std::uint32_t frame_count_{};
};
}
