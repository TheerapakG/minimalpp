#ifndef MINPP_TUPLE_H_
#define MINPP_TUPLE_H_
#include "minpp/_minpp_macros.h"
#include "minpp/common_meta.h"

#include <concepts>
#include <type_traits>
#include <utility>

MINPP_IMPL_BEGIN

template<std::size_t I, typename T>
struct tuple_leaf {
    using type = T;
    T value{};
};

template<std::size_t I>
struct tuple_t_from_size {
    template<std::size_t... Is>
    struct _tuple_t_with_size {
        template<typename... T>
        struct _tuple_t: public tuple_leaf<Is, T>... {
            // 1)
            constexpr _tuple_t() = default;
            // 2)
            constexpr _tuple_t(const T&... v) : tuple_leaf<Is, T>{v}... {};
            // 3)
            template <typename... U>
            constexpr _tuple_t(U&&... u) requires requires {
                requires sizeof...(U) == sizeof...(T);
            } : tuple_leaf<Is, T>{std::forward<U>(u)}... {};
            // 4)
            _tuple_t(const _tuple_t&) = default;
            // 5)
            _tuple_t(_tuple_t&&) = default;
            // 6, 8)
            template <template <typename...> typename _Tuple_Like, typename... UTypes>
            constexpr _tuple_t(const _Tuple_Like<UTypes...>& v) : tuple_leaf<Is, T>{get<Is>(v)}... {};
            // 7, 9)
            template <template <typename...> typename _Tuple_Like, typename... UTypes>
            constexpr _tuple_t(_tuple_t<UTypes...>&& v) : tuple_leaf<Is, T>{std::forward<UTypes&&>(get<Is>(v))}... {};

            // not annotated

            _tuple_t& operator=(const _tuple_t& other) noexcept((std::is_nothrow_copy_assignable_v<T> && ...)) {
                (tuple_leaf<Is, T>::operator=(static_cast<const tuple_leaf<Is, T>& >(other)), ...);
                return *this;
            }

            _tuple_t& operator=(_tuple_t&& other) noexcept((std::is_nothrow_move_assignable_v<T> && ...)) {
                (tuple_leaf<Is, T>::operator=(std::forward<tuple_leaf<Is, T>&&>(other)), ...);
                return *this;
            }
        };
    };

    template<typename... T>
    using tuple_t = typename make_same_parameterized_value<std::size_t>::of_t<_tuple_t_with_size, std::make_integer_sequence<std::size_t, I>>::template _tuple_t<T...>;
};

template<std::size_t I, typename T>
T _impl_typeof_helper(const tuple_leaf<I, T>&);

template<std::size_t I, typename T>
constexpr T& _impl_at(tuple_leaf<I, T>& leaf) noexcept {
    return leaf.value;
}

template<std::size_t I, typename T>
constexpr const T& _impl_at(const tuple_leaf<I, T>& leaf) noexcept {
    return leaf.value;
}

template<std::size_t I, typename T>
constexpr T&& _impl_at(tuple_leaf<I, T>&& leaf) noexcept {
    return static_cast<T&&>(leaf.value);
}

template<std::size_t I, typename T>
constexpr const T&& _impl_at(const tuple_leaf<I, T>&& leaf) noexcept {
    return static_cast<const T&&>(leaf.value);
}


template<typename T, std::size_t I>
constexpr T& _impl_at_type_leaf(tuple_leaf<I, T>& leaf) noexcept {
    return leaf.value;
}

template<typename T, std::size_t I>
constexpr const T& _impl_at_type_leaf(const tuple_leaf<I, T>& leaf) noexcept {
    return leaf.value;
}

template<typename T, std::size_t I>
constexpr T&& _impl_at_type_leaf(tuple_leaf<I, T>&& leaf) noexcept {
    return static_cast<T&&>(leaf.value);
}

template<typename T, std::size_t I>
constexpr const T&& _impl_at_type_leaf(const tuple_leaf<I, T>&& leaf) noexcept {
    return static_cast<const T&&>(leaf.value);
}

MINPP_IMPL_END

MINPP_NAMESPACE_BEGIN

template<typename... Types>
struct tuple: public impl::tuple_t_from_size<sizeof...(Types)>::template tuple_t<Types...> {

    using _impl = typename impl::tuple_t_from_size<sizeof...(Types)>::template tuple_t<Types...>;
    
    /*
    § 20.5.3.1 
    1)  constexpr explicit(see below ) tuple();
        Effects: Value-initializes each element.
        Remarks: The expression inside explicit evaluates to true if and only if Ti is not copy-list-initializable
        from an empty list for at least one i.
    */
    constexpr explicit(!(minpp::is_list_constructible_v<Types> && ...)) tuple() requires requires {
        requires (std::constructible_from<Types, Types> && ...);
    } = default;

    /*
    § 20.5.3.1 
    2)  constexpr explicit(see below ) tuple(const Types&...); // only if sizeof...(Types) >= 1
        Effects: Initializes each element with the value of the corresponding parameter.
        Remarks: The expression inside explicit is equivalent to:
            !conjunction_v<is_convertible<const Types&, Types>...>
    */
    constexpr explicit(!(std::is_convertible_v<const Types&, Types> && ...)) tuple(const Types&... v) requires requires {
        // requires sizeof...(Types) > 0; // sizeof...(Types) == 0 instantiate specialized tuple template
        requires (std::copy_constructible<Types> && ...);
    }
    : _impl{v...} {};

    /*
    § 20.5.3.1 
    3)  template<class... UTypes>
        constexpr explicit(see below ) tuple(UTypes&&...); // only if sizeof...(Types) >= 1
        Effects: Initializes the elements in the tuple with the corresponding value in std::forward<UTypes>(u).
        Remarks: The expression inside explicit is equivalent to:
            !conjunction_v<is_convertible<UTypes, Types>...>
    */
    template <typename... UTypes>
    constexpr explicit(!(std::is_convertible_v<UTypes, Types> && ...)) tuple(UTypes&&... u) requires requires {
        requires sizeof...(Types) == sizeof...(UTypes);
        // requires sizeof...(Types) > 0; // sizeof...(Types) == 0 instantiate specialized tuple template
        requires (std::constructible_from<Types, UTypes> && ...);
    }
    : _impl{std::forward<UTypes>(u)...} {};

    /*
    § 20.5.3.1 
    4)  tuple(const tuple&) = default;
        Effects: Initializes each element of *this with the corresponding element of u.
    */
    tuple(const tuple&) requires requires {
        requires (std::copy_constructible<Types> && ...);
    } = default;

    /*
    § 20.5.3.1 
    5)  tuple(tuple&&) = default;
        Effects: For all i, initializes the ith element of *this with std::forward<Ti>(get<i>(u))
    */
    tuple(tuple&&) requires requires {
        requires (std::move_constructible<Types> && ...);
    } = default;

    /*
    § 20.5.3.1 
    6)  template<class... UTypes> 
        constexpr explicit(see below) tuple(const tuple<UTypes...>& u);
        Effects: Initializes each element of *this with the corresponding element of u.
        Remarks: The expression inside explicit is equivalent to:
            !conjunction_v<is_convertible<const UTypes&, Types>...>
    */
    template <typename... UTypes>
    constexpr explicit(!(std::is_convertible_v<const UTypes&, Types> && ...)) tuple(const tuple<UTypes...>& v) requires requires {
        requires sizeof...(Types) == sizeof...(UTypes);
        requires (std::constructible_from<Types, const UTypes&> && ...);
        requires requires {
            requires sizeof...(Types) != 1 ||
            requires {
                requires !(std::convertible_to<const tuple<UTypes>&, Types> && ...);
                requires !(std::constructible_from<Types, const tuple<UTypes>&> && ...);
                !std::same_as<tuple<Types...>, tuple<UTypes...>>;
            };
        };
    }
    : _impl{v} {}

#if MINPP_STD_COMPAT

    template <typename... UTypes>
    constexpr explicit(!(std::is_convertible_v<const UTypes&, Types> && ...)) tuple(const std::tuple<UTypes...>& v) requires requires {
        requires sizeof...(Types) == sizeof...(UTypes);
        requires (std::constructible_from<Types, const UTypes&> && ...);
        requires requires {
            requires sizeof...(Types) != 1 ||
            requires {
                requires !(std::convertible_to<const tuple<UTypes>&, Types> && ...);
                requires !(std::constructible_from<Types, const tuple<UTypes>&> && ...);
                !std::same_as<tuple<Types...>, tuple<UTypes...>>;
            };
        };
    }
    : _impl{v} {}

#endif

    /*
    § 20.5.3.1 
    7)  template<class... UTypes> 
        constexpr explicit(see below) tuple(tuple<UTypes...>&& u);
        Effects: For all i, initializes the ith element of *this with std::forward<Ui>(get<i>(u)).
        Remarks: The expression inside explicit is equivalent to:
            !conjunction_v<is_convertible<UTypes, Types>...>
    */
    template <typename... UTypes>
    constexpr explicit(!(std::is_convertible_v<UTypes, Types> && ...)) tuple(tuple<UTypes...>&& v) requires requires {
        requires sizeof...(Types) == sizeof...(UTypes);
        requires (std::constructible_from<Types, UTypes> && ...);
        requires requires {
            requires sizeof...(Types) != 1 ||
            requires {
                requires !(std::convertible_to<tuple<UTypes>, Types> && ...);
                requires !(std::constructible_from<Types, tuple<UTypes>> && ...);
                !std::same_as<tuple<Types...>, tuple<UTypes...>>;
            };
        };
    }
    : _impl{std::forward<tuple<UTypes...>&&>(v)} {}

#if MINPP_STD_COMPAT

    template <typename... UTypes>
    constexpr explicit(!(std::is_convertible_v<UTypes, Types> && ...)) tuple(std::tuple<UTypes...>&& v) requires requires {
        requires sizeof...(Types) == sizeof...(UTypes);
        requires (std::constructible_from<Types, UTypes> && ...);
        requires requires {
            requires sizeof...(Types) != 1 ||
            requires {
                requires !(std::convertible_to<tuple<UTypes>, Types> && ...);
                requires !(std::constructible_from<Types, tuple<UTypes>> && ...);
                !std::same_as<tuple<Types...>, tuple<UTypes...>>;
            };
        };
    }
    : _impl{std::forward<tuple<UTypes...>&&>(v)} {}

#endif

    /*
    § 20.5.3.1 
    8)  template<class U1, class U2> 
        constexpr explicit(see below) tuple(const pair<U1, U2>& u); // only if sizeof...(Types) == 2
        Effects: Initializes the first element with u.first and the second element with u.second.
        Remarks: The expression inside explicit is equivalent to:
            !is_convertible_v<const U1&, T0> || !is_convertible_v<const U2&, T1>
    */
    template <typename... UTypes>
    constexpr explicit(!(std::is_convertible_v<const UTypes&, Types> && ...)) tuple(const std::pair<UTypes...>&) requires requires {
        requires sizeof...(Types) == 2;
        requires (std::constructible_from<Types, const UTypes&> && ...);
    }
    {}

    /*
    § 20.5.3.1 
    9)  template<class U1, class U2> 
        constexpr explicit(see below) tuple(pair<U1, U2>&& u); // only if sizeof...(Types) == 2
        Effects: Initializes the first element with std::forward<U1>(u.first) and the second element with
            std::forward<U2>(u.second).
        Remarks: The expression inside explicit is equivalent to:
            !is_convertible_v<U1, T0> || !is_convertible_v<U2, T1>
    */
    template <typename... UTypes>
    constexpr explicit(!(std::is_convertible_v<const UTypes&, Types> && ...)) tuple(std::pair<UTypes...>&&) requires requires {
        requires sizeof...(Types) == 2;
        requires (std::constructible_from<Types, UTypes> && ...);
    }
    {}

    // not annotated

    tuple& operator=(const tuple& other) noexcept((std::is_nothrow_copy_assignable_v<Types> && ...)) {
        _impl::operator=(other);
        return *this;
    }

    tuple& operator=(tuple&& other) noexcept((std::is_nothrow_move_assignable_v<Types> && ...)) {
        _impl::operator=(std::forward<_impl&&>(other));
        return *this;
    }
};

/*
specialization for tuple with 0 types
*/
template <>
struct tuple<> {
    constexpr tuple() noexcept = default;
    constexpr tuple(const tuple&) noexcept = default;
    constexpr tuple(tuple&&) noexcept = default;

    constexpr tuple& operator=(const tuple&) noexcept = default;
    constexpr tuple& operator=(tuple&&) noexcept = default;
};

MINPP_NAMESPACE_END

MINPP_IMPL_BEGIN

template <typename T, typename... Ts>
concept Once_T = requires {
    requires count_v<T, Ts...> == 1; // T have to appear in Ts exactly once
};


template <typename T, typename... Ts>
constexpr decltype(auto) _impl_at_type_l(tuple<Ts...>& t) noexcept requires impl::Once_T<T, Ts...> {
    return _impl_at_type_leaf<T>(t);
}

template <typename T, typename... Ts>
constexpr decltype(auto) _impl_at_type_cl(const tuple<Ts...>& t) noexcept requires impl::Once_T<T, Ts...> {
    return _impl_at_type_leaf<T>(t);
}

template <typename T, typename... Ts>
constexpr decltype(auto) _impl_at_type_r(tuple<Ts...>&& t) noexcept requires impl::Once_T<T, Ts...> {
    return _impl_at_type_leaf<T>(std::forward<minpp::tuple<Ts...>&&>(t));
}

template <typename T, typename... Ts>
constexpr decltype(auto) _impl_at_type_cr(const tuple<Ts...>&& t) noexcept requires impl::Once_T<T, Ts...> {
    return _impl_at_type_leaf<T>(std::forward<const minpp::tuple<Ts...>&&>(t));
}

MINPP_IMPL_END

STD_BEGIN

template <typename... T>
struct tuple_size<minpp::tuple<T...>> : std::integral_constant<std::size_t, sizeof...(T)> {};

template <std::size_t I, typename... T>
struct tuple_element<I, minpp::tuple<T...>> {
    using type = decltype(_impl_typeof_helper<I>(minpp::tuple<T...>{}));
};

STD_END

MINPP_NAMESPACE_BEGIN

template <std::size_t I, typename... Ts>
constexpr decltype(auto) get(minpp::tuple<Ts...>& t) noexcept {
    return minpp::impl::_impl_at<I>(t);
}

template <std::size_t I, typename... Ts>
constexpr decltype(auto) get(const minpp::tuple<Ts...>& t) noexcept {
    return minpp::impl::_impl_at<I>(t);
}

template <std::size_t I, typename... Ts>
constexpr decltype(auto) get(minpp::tuple<Ts...>&& t) noexcept {
    return minpp::impl::_impl_at<I>(std::forward<minpp::tuple<Ts...>&&>(t));
}

template <std::size_t I, typename... Ts>
constexpr decltype(auto) get(const minpp::tuple<Ts...>&& t) noexcept {
    return minpp::impl::_impl_at<I>(std::forward<const minpp::tuple<Ts...>&&>(t));
}


template <typename T, typename... Ts>
constexpr decltype(auto) get(minpp::tuple<Ts...>& t) noexcept {
    return minpp::impl::_impl_at_type_l<T>(t);
}

template <typename T, typename... Ts>
constexpr decltype(auto) get(const minpp::tuple<Ts...>& t) noexcept {
    return minpp::impl::_impl_at_type_cl<T>(t);
}

template <typename T, typename... Ts>
constexpr decltype(auto) get(minpp::tuple<Ts...>&& t) noexcept {
    return minpp::impl::_impl_at_type_r<T>(std::forward<minpp::tuple<Ts...>&&>(t));
}

template <typename T, typename... Ts>
constexpr decltype(auto) get(const minpp::tuple<Ts...>&& t) noexcept {
    return minpp::impl::_impl_at_type_cr<T>(std::forward<const minpp::tuple<Ts...>&&>(t));
}

MINPP_NAMESPACE_END

#endif
