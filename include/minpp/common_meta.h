#ifndef MINPP_COMMON_META_H_
#define MINPP_COMMON_META_H_

#include "minpp/_minpp_macros.h"

#include <utility>

MINPP_NAMESPACE_BEGIN

template <typename... Args>
struct template_holder {};

MINPP_NAMESPACE_END

MINPP_IMPL_BEGIN

template <typename T, typename Args, typename=void>
struct _is_list_constructible : public std::false_type {};

template <typename T, typename... Args>
struct _is_list_constructible<
    T, 
    template_holder<Args...>, 
    std::void_t <
        decltype( T{ std::declval<Args>()... } )
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
using is_list_constructible = typename impl::_is_list_constructible<T, template_holder<Args...>>;

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

MINPP_NAMESPACE_END

#endif
