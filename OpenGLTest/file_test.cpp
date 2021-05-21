#include <gtest/gtest.h>

#include "utils/file.cpp"

namespace {
	using namespace file;

	TEST(FileTest, TestReadFile) {
		constexpr auto* data = R"(
			The quick brown
			fox jumps over
			the lazy dog
		)";
		std::stringstream ss{data};
		ASSERT_EQ(Read(ss), data);
	}
}