#include <gtest/gtest.h>

#include "geometry/vertex.h"

using namespace geometry;

namespace {
	TEST(VertexTest, TestInsertionOperator) {
		std::ostringstream oss;
		oss << Vertex{42, glm::vec4{}, glm::vec3{}};
		ASSERT_EQ("42", oss.str());
	}
}