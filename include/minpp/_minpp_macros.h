#ifndef _MINPP_MACROS_H
#define _MINPP_MACROS_H

#define MINPP_NAMESPACE_BEGIN namespace minpp {
#define MINPP_NAMESPACE_END }

#define MINPP_IMPL_BEGIN namespace minpp::impl {
#define MINPP_IMPL_END }

#define STD_BEGIN namespace std {
#define STD_END }

#ifndef MINPP_STD_COMPAT
#define MINPP_STD_COMPAT true
#endif

#endif