#ifndef MINPP_COMMON_SHORTHANDS_H_
#define MINPP_COMMON_SHORTHANDS_H_

#include "minpp/_minpp_macros.h"

#include <utility>

MINPP_SUBSPACE_BEGIN(shorthands)

template<int I>
using in_c = std::integral_constant<int, I>;

template<std::size_t I>
using id_c = std::integral_constant<std::size_t, I>;

MINPP_SUBSPACE_END

#endif