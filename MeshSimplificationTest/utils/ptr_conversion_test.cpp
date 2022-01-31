#include "utils/ptr_conversion.h"

#include <memory>
#include <stdexcept>

#include <gtest/gtest.h>

namespace {
using namespace utils;
using namespace std;

TEST(PtrConversion, TestValidWeakPointerConversionToSharedPointer) {
	const auto meaning_of_life = make_shared<const int>(42);
	const std::weak_ptr weak_ptr = meaning_of_life;
	ASSERT_EQ(42, *Get(weak_ptr));
}

TEST(PtrConversion, TestInvalidWeakPointerConversionToSharedPointer) {
	std::weak_ptr<const int> weak_ptr;
	{
		const auto meaning_of_life = make_shared<const int>(42);
		weak_ptr = meaning_of_life;
	}
	ASSERT_THROW(Get(weak_ptr), runtime_error);
}
}
