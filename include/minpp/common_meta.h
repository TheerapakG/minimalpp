#ifndef MINPP_COMMON_META_H_
#define MINPP_COMMON_META_H_

#include "minpp/_minpp_macros.h"

#include <utility>

MINPP_NAMESPACE_BEGIN

template <typename... Args>
struct template_type_holder {};

template <typename T>
struct template_value_holder {
  template <T... Args>
  struct type {};
};

template <typename T, T... Args>
using template_value_holder_t = typename template_value_holder<T>::template type<Args...>;

MINPP_NAMESPACE_END

MINPP_IMPL_BEGIN

template <typename T, typename Args, typename=void>
struct _is_list_constructible : public std::false_type {};

template <typename T, typename... Args>
struct _is_list_constructible<
  T, 
  template_type_holder<Args...>, 
  ::std::void_t <
    decltype( T{ ::std::declval<Args>()... } )
  >
> : public std::true_type {};

MINPP_IMPL_END

MINPP_NAMESPACE_BEGIN

/*
Make T parameterized by the same template argument type of U
*/
template <template <typename...> typename T, typename U>
struct make_same_parameterized_type {}; //static_assert(false, "U is not a parameterized template");

/*
Make T parameterized by the same template argument type of U
*/
template <template <typename...> typename T, template <typename...> typename _U, typename... _UTypes>
struct make_same_parameterized_type<T, _U<_UTypes...>> {
  using type = T<_UTypes...>;
};

/*
Make T parameterized by the same template argument type of U
*/
template <template <typename...> typename T, typename U>
using make_same_parameterized_type_t = typename make_same_parameterized_type<T, U>::type;


/*
Make a template parameterized by the same template argument value of other parameterized template
*/
template <typename Value_T>
struct make_same_parameterized_value {    
  /*
  Make T parameterized by the same template argument value of U
  */
  template <template <Value_T...> typename T, typename U>
  struct of {}; // static_assert(false, "U is not a parametized template");

  /*
  Make T parameterized by the same template argument value of U
  */
  template <template <Value_T...> typename T, template <Value_T...> typename _U, Value_T... _UTypes>
  struct of <T, _U<_UTypes...>> {
    using type = T<_UTypes...>;
  };

  /*
  Make T parameterized by the same template argument value of U (U is an std::integer_sequence)
  */
  template <template <Value_T...> typename T, Value_T... _UTypes>
  struct of <T, std::integer_sequence<Value_T, _UTypes...>> {
    using type = T<_UTypes...>;
  };

  /*
  Make T parameterized by the same template argument value of U
  */
  template <template <Value_T...> typename T, typename U>
  using of_t = typename of<T, U>::type;
};

template <typename T, typename... Args>
using is_list_constructible = typename impl::_is_list_constructible<T, template_type_holder<Args...>>;

template <typename T, typename... Args>
static constexpr bool is_list_constructible_v = is_list_constructible<T, Args...>::value;

template <typename T, T... Value_T>
struct sum : public std::integral_constant<T, (Value_T + ...)> {};

template <typename T, T... Value_T>
static constexpr T sum_v = sum<T, Value_T...>::value;

template <typename T, typename... Ts>
struct count: public sum<std::size_t, (std::is_same_v<T, Ts>?1:0)...> {};

template <typename T, typename... Ts>
static constexpr T count_v = count<T, Ts...>::value;

/*
Make a template parameterized by the template arguments types of other parameterized templates
*/
template <template <typename...> typename Container_T>
struct flatten_type {
  /*
  Make Container_T parameterized by the same template argument values of Us
  */
  template <typename... Us>
  struct apply {}; // static_assert(false, "U is not a parametized template");

  /*
  Make Container_T parameterized by the same template argument types of Us
  */
  template <template <typename...> typename _U, typename... _UTypes>
  struct apply<_U<_UTypes...>> {
    using type = Container_T<_UTypes...>;
  };

  /*
  Make Container_T parameterized by the same template argument types of Us
  */
  template <template <typename...> typename _U1, typename... _U1Types, template <typename...> typename _U2, typename... _U2Types, typename... Us>
  struct apply<_U1<_U1Types...>, _U2<_U2Types...>, Us...> {
    using type = typename apply<template_type_holder<_U1Types..., _U2Types...>, Us...>::type;
  };

  /*
  Make T parameterized by the same template argument value of U
  */
  template <typename... Us>
  using apply_t = typename apply<Us...>::type;
};

/*
Make a template parameterized by the template arguments values of other parameterized templates
*/
template <typename Value_T>
struct flatten_value {
  template <template <Value_T...> typename Container_T>
  struct in {
    /*
    Make Container_T parameterized by the same template argument values of Us
    */
    template <typename... Us>
    struct apply {}; // static_assert(false, "U is not a parametized template");

    /*
    Make Container_T parameterized by the same template argument values of Us
    */
    template <template <Value_T...> typename _U, Value_T... _UTypes>
    struct apply<_U<_UTypes...>> {
      using type = Container_T<_UTypes...>;
    };

    /*
    Make Container_T parameterized by the same template argument values of Us
    */
    template <template <Value_T...> typename _U1, Value_T... _U1Types, template <Value_T...> typename _U2, Value_T... _U2Types, typename... Us>
    struct apply<_U1<_U1Types...>, _U2<_U2Types...>, Us...> {
      using type = typename apply<template_value_holder_t<Value_T, _U1Types..., _U2Types...>, Us...>::type;
    };

    /*
    Make Container_T parameterized by the same template argument values of Us (U is an std::integer_sequence)
    */
    template <Value_T... _UTypes>
    struct apply<std::integer_sequence<Value_T, _UTypes...>> {
      using type = Container_T<_UTypes...>;
    };

    /*
    Make Container_T parameterized by the same template argument values of Us (U is an std::integer_sequence)
    */
    template <Value_T... _U1Types, Value_T... _U2Types, typename... Us>
    struct apply<std::integer_sequence<Value_T, _U1Types...>, std::integer_sequence<Value_T, _U2Types...>, Us...> {
      using type = typename apply<template_value_holder_t<Value_T, _U1Types..., _U2Types...>, Us...>::type;
    };

    /*
    Make Container_T parameterized by the same template argument values of Us (U is an std::integer_sequence)
    */
    template <template <Value_T...> typename _U1, Value_T... _U1Types, Value_T... _U2Types, typename... Us>
    struct apply<_U1<_U1Types...>, std::integer_sequence<Value_T, _U2Types...>, Us...> {
      using type = typename apply<template_value_holder_t<Value_T, _U1Types..., _U2Types...>, Us...>::type;
    };

    /*
    Make Container_T parameterized by the same template argument values of Us (U is an std::integer_sequence)
    */
    template <Value_T... _U1Types, template <Value_T...> typename _U2, Value_T... _U2Types, typename... Us>
    struct apply<std::integer_sequence<Value_T, _U1Types...>, _U2<_U2Types...>, Us...> {
      using type = typename apply<template_value_holder_t<Value_T, _U1Types..., _U2Types...>, Us...>::type;
    };

    /*
    Make T parameterized by the same template argument value of U
    */
    template <typename... Us>
    using apply_t = typename apply<Us...>::type;
  };
};

/*
Make a integer_sequence parameterized by the template arguments values of other parameterized templates
*/
template <typename Value_T>
struct flatten_value_in_integer_sequence {
  /*
  Make Container_T parameterized by the same template argument values of Us
  */
  template <typename... Us>
  struct apply {}; // static_assert(false, "U is not a parametized template");

  /*
  Make Container_T parameterized by the same template argument values of Us
  */
  template <template <Value_T...> typename _U, Value_T... _UTypes>
  struct apply<_U<_UTypes...>> {
    using type = std::integer_sequence<Value_T, _UTypes...>;
  };

  /*
  Make Container_T parameterized by the same template argument values of Us
  */
  template <template <Value_T...> typename _U1, Value_T... _U1Types, template <Value_T...> typename _U2, Value_T... _U2Types, typename... Us>
  struct apply<_U1<_U1Types...>, _U2<_U2Types...>, Us...> {
    using type = typename apply<template_value_holder_t<Value_T, _U1Types..., _U2Types...>, Us...>::type;
  };

  /*
  Make Container_T parameterized by the same template argument values of Us (U is an std::integer_sequence)
  */
  template <Value_T... _UTypes>
  struct apply<std::integer_sequence<Value_T, _UTypes...>> {
    using type = std::integer_sequence<Value_T, _UTypes...>;
  };

  /*
  Make Container_T parameterized by the same template argument values of Us (U is an std::integer_sequence)
  */
  template <Value_T... _U1Types, Value_T... _U2Types, typename... Us>
  struct apply<std::integer_sequence<Value_T, _U1Types...>, std::integer_sequence<Value_T, _U2Types...>, Us...> {
    using type = typename apply<template_value_holder_t<Value_T, _U1Types..., _U2Types...>, Us...>::type;
  };

  /*
  Make Container_T parameterized by the same template argument values of Us (U is an std::integer_sequence)
  */
  template <template <Value_T...> typename _U1, Value_T... _U1Types, Value_T... _U2Types, typename... Us>
  struct apply<_U1<_U1Types...>, std::integer_sequence<Value_T, _U2Types...>, Us...> {
    using type = typename apply<template_value_holder_t<Value_T, _U1Types..., _U2Types...>, Us...>::type;
  };

  /*
  Make Container_T parameterized by the same template argument values of Us (U is an std::integer_sequence)
  */
  template <Value_T... _U1Types, template <Value_T...> typename _U2, Value_T... _U2Types, typename... Us>
  struct apply<std::integer_sequence<Value_T, _U1Types...>, _U2<_U2Types...>, Us...> {
    using type = typename apply<template_value_holder_t<Value_T, _U1Types..., _U2Types...>, Us...>::type;
  };

  /*
  Make T parameterized by the same template argument value of U
  */
  template <typename... Us>
  using apply_t = typename apply<Us...>::type;
};

MINPP_NAMESPACE_END

MINPP_IMPL_BEGIN

template <::std::size_t V, ::std::size_t N>
struct _make_index_duplicates_t {
  using type = flatten_value_in_integer_sequence<::std::size_t>::apply<template_value_holder_t<::std::size_t, V>, typename _make_index_duplicates_t<V, N-1>::type>;
};

template <::std::size_t V>
struct _make_index_duplicates_t<V, 0> {
  using type = std::index_sequence<>;
};

MINPP_IMPL_END

MINPP_NAMESPACE_BEGIN

template <::std::size_t V, ::std::size_t N>
using make_index_duplicates = typename impl::_make_index_duplicates_t<V, N>::type;

MINPP_NAMESPACE_END

#endif
