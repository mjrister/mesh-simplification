#pragma once

#include <memory>
#include <stdexcept>

namespace ptr {

/**
 * \brief Coverts a weak pointer to a shared pointer.
 * \tparam T The pointer type.
 * \param weak_t The weak pointer to convert.
 * \return A shared pointer that refers to the object managed by \p weak_t.
 * \throw std::runtime_error Indicates the weak pointer is expired.
 */
template <typename T>
static std::shared_ptr<T> Get(const std::weak_ptr<T>& weak_t) {
	if (auto shared_t = weak_t.lock()) return shared_t;
	throw std::runtime_error{"Attempted to access a dangling pointer"};
}
}
