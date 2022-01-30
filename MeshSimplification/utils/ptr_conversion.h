#pragma once

#include <memory>
#include <stdexcept>

namespace ptr {

/**
 * \brief Converts a weak pointer to a shared pointer.
 * \tparam T The shared pointer type.
 * \param weak_t The weak pointer to convert.
 * \return The weak pointer converted to a shared pointer.
 * \throw std::runtime_error Indicates the weak pointer is expired.
 */
template <typename T>
static std::shared_ptr<T> Get(const std::weak_ptr<T>& weak_t) {
	if (auto shared_t = weak_t.lock()) return shared_t;
	throw std::runtime_error{"Attempted to access a dangling pointer"};
}
}
