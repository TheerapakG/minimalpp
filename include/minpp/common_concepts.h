#ifndef MINPP_COMMON_CONCEPTS_H_
#define MINPP_COMMON_CONCEPTS_H_

#include "minpp/_minpp_macros.h"

#include <concepts>
#include <memory>

MINPP_NAMESPACE_BEGIN

template <typename T, typename Alloc, typename... Args>
concept leading_allocator_constructible = requires {
  requires std::uses_allocator_v<T, Alloc>;
  std::constructible_from<T, std::allocator_arg_t, Alloc, Args...>;
};

template <typename T, typename Alloc, typename... Args>
concept trailing_allocator_constructible = requires {
  requires std::uses_allocator_v<T, Alloc>;
  std::constructible_from<T, Args..., Alloc>;
};

MINPP_NAMESPACE_END

#endif
