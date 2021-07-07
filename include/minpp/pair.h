#ifndef MINPP_PAIR_H_
#define MINPP_PAIR_H_
#include "minpp/_minpp_macros.h"

MINPP_NAMESPACE_BEGIN

template<typename _first_t, typename _second_t>
struct pair {
    using first_t = _first_t;
    using second_t = _second_t;

    first_t first;
    second_t second;

    auto operator<=>(const pair&) = default;
};

MINPP_NAMESPACE_END

#endif
