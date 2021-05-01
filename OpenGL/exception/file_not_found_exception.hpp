#pragma once

#include <sstream>
#include <stdexcept>

class FileNotFoundException final : std::exception {

public:
	explicit FileNotFoundException(const std::string_view& filepath)
		: filepath_{filepath} {}

	[[nodiscard]] const char* what() const override {
		std::ostringstream oss;
		oss << "Unable to open " << filepath_;
		const auto message = oss.str();
		return message.c_str();
	}

private:
	std::string_view filepath_;
};
