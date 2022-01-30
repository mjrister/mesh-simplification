#pragma once

#include <memory>
#include <stdexcept>

namespace ptr {

template <typename T>
[[nodiscard]] static std::shared_ptr<T> Get(const std::weak_ptr<T>& weak_t) {
	if (auto shared_t = weak_t.lock()) return shared_t;
	throw std::runtime_error{"Attempted to access a dangling pointer"};
}
}
