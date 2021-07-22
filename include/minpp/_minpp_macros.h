#ifndef _MINPP_MACROS_H
#define _MINPP_MACROS_H

#define MINPP_NAMESPACE_BEGIN namespace minpp {
#define MINPP_NAMESPACE_END }

#define MINPP_SUBSPACE_BEGIN(subspace) namespace minpp::subspace {
#define MINPP_SUBSPACE_END }

#define MINPP_IMPL_BEGIN MINPP_SUBSPACE_BEGIN(impl)
#define MINPP_IMPL_END MINPP_SUBSPACE_END

#define MINPP_STD_BEGIN namespace std {
#define MINPP_STD_END }

#ifndef MINPP_STD_COMPAT
#define MINPP_STD_COMPAT true
#endif

#endif