#ifndef MINPP_TUPLE_H_
#define MINPP_TUPLE_H_
#include "minpp/_minpp_macros.h"
#include "minpp/common_meta.h"

#include <concepts>
#include <type_traits>
#include <utility>

MINPP_NAMESPACE_BEGIN

template<typename... Types>
struct tuple;

MINPP_NAMESPACE_END

MINPP_IMPL_BEGIN

struct _select_tuple_leaf_ctor {};

template<std::size_t I, typename T>
struct tuple_leaf {
    private:
    T value;

    template<std::size_t, typename>
    friend struct tuple_leaf;

    template<std::size_t _I, typename _T>
    friend constexpr _T& _impl_at(tuple_leaf<_I, _T>& leaf) noexcept;
    template<std::size_t _I, typename _T>
    friend constexpr const _T& _impl_at(const tuple_leaf<_I, _T>& leaf) noexcept;
    template<std::size_t _I, typename _T>
    friend constexpr _T&& _impl_at(tuple_leaf<_I, _T>&& leaf) noexcept;
    template<std::size_t _I, typename _T>
    friend constexpr const _T&& _impl_at(const tuple_leaf<_I, _T>&& leaf) noexcept;

    template<typename _T, std::size_t _I>
    friend constexpr _T& _impl_at_type_leaf(tuple_leaf<_I, _T>& leaf) noexcept;
    template<typename _T, std::size_t _I>
    friend constexpr const _T& _impl_at_type_leaf(const tuple_leaf<_I, _T>& leaf) noexcept;
    template<typename _T, std::size_t _I>
    friend constexpr _T&& _impl_at_type_leaf(tuple_leaf<_I, _T>&& leaf) noexcept;
    template<typename _T, std::size_t _I>
    friend constexpr const _T&& _impl_at_type_leaf(const tuple_leaf<_I, _T>&& leaf) noexcept;

    public:
    constexpr tuple_leaf() = default;

    template<typename U>
    constexpr tuple_leaf(U&& v): value{std::forward<U>(v)} {}

    template<typename U>
    constexpr tuple_leaf(_select_tuple_leaf_ctor, const tuple_leaf<I, U>& v): value{v.value} {}

    template<typename U>
    constexpr tuple_leaf(_select_tuple_leaf_ctor, tuple_leaf<I, U>&& v): value{std::move(v.value)} {}

    constexpr void swap(tuple_leaf& other) noexcept(std::is_nothrow_swappable_v<T>) {
        std::swap(value, other.value);
    }
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
            constexpr _tuple_t(const T&... v) : tuple_leaf<Is, T>{v}... {}
            // 3)
            template <typename... UTypes>
            constexpr _tuple_t(UTypes&&... u) requires requires {
                requires sizeof...(UTypes) == sizeof...(T);
            } : tuple_leaf<Is, T>{std::forward<UTypes>(u)}... {}
            // 4)
            _tuple_t(const _tuple_t&) = default;
            // 5)
            _tuple_t(_tuple_t&&) = default;
            // 6, 8)
            template <template <typename...> typename _Tuple_Like, typename... UTypes>
            constexpr _tuple_t(const _Tuple_Like<UTypes...>& v) requires requires {
                requires !std::derived_from<_Tuple_Like<UTypes...>, _tuple_t<UTypes...>>;
                { ((get<Is>(v), void()), ...) };
            }
            : tuple_leaf<Is, T>{get<Is>(v)}... {};

            template <typename... UTypes>
            constexpr _tuple_t(const _tuple_t<UTypes...>& v) : tuple_leaf<Is, T>{_select_tuple_leaf_ctor{}, v}... {};
            // 7, 9)
            template <template <typename...> typename _Tuple_Like, typename... UTypes>
            constexpr _tuple_t(_Tuple_Like<UTypes...>&& v) requires requires {
                requires !std::derived_from<_Tuple_Like<UTypes...>, _tuple_t<UTypes...>>;
                { ((get<Is>(v), void()), ...) };
            }
            : tuple_leaf<Is, T>{get<Is>(v)}... {};

            template <typename... UTypes>
            constexpr _tuple_t(_tuple_t<UTypes...>&& v) : tuple_leaf<Is, T>{_select_tuple_leaf_ctor{}, v}... {};

            // not annotated

            constexpr _tuple_t& operator=(const _tuple_t& other) noexcept((std::is_nothrow_copy_assignable_v<T> && ...)) {
                (tuple_leaf<Is, T>::operator=(static_cast<const tuple_leaf<Is, T>&>(other)), ...);
                return *this;
            }

            constexpr _tuple_t& operator=(_tuple_t&& other) noexcept((std::is_nothrow_move_assignable_v<T> && ...)) {
                (tuple_leaf<Is, T>::operator=(static_cast<tuple_leaf<Is, T>&&>(other)), ...);
                return *this;
            }

            constexpr void swap(_tuple_t& other) noexcept((std::is_nothrow_swappable_v<T> && ...)) {
                (tuple_leaf<Is, T>::swap(other), ...);
            }
        };
    };

    template<typename... T>
    using tuple_t = typename make_same_parameterized_value<std::size_t>::of_t<_tuple_t_with_size, std::make_index_sequence<I>>::template _tuple_t<T...>;
};

struct ignore_t
{
    template <typename T>
    constexpr ignore_t& operator=(T&&) { return *this; }
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
    1)  
    Effects: Value-initializes each element.
    Remarks: The expression inside explicit evaluates to true if and only if Ti is not copy-list-initializable
        from an empty list for at least one i.
    @fn constexpr explicit(see below ) tuple();
    */
    constexpr explicit(!(minpp::is_list_constructible_v<Types> && ...)) tuple() requires requires {
        requires (std::constructible_from<Types, Types> && ...);
    } = default;

    /*
    § 20.5.3.1 
    2)      
    Effects: Initializes each element with the value of the corresponding parameter.
    Remarks: The expression inside explicit is equivalent to:
            !conjunction_v<is_convertible<const Types&, Types>...>
    @fn constexpr explicit(see below ) tuple(const Types&...); // only if sizeof...(Types) >= 1
    */
    constexpr explicit(!(std::is_convertible_v<const Types&, Types> && ...)) tuple(const Types&... v) requires requires {
        // requires sizeof...(Types) > 0; // sizeof...(Types) == 0 instantiate specialized tuple template
        requires (std::copy_constructible<Types> && ...);
    }
    : _impl{v...} {};

    /*
    § 20.5.3.1 
    3) 
    Effects: Initializes the elements in the tuple with the corresponding value in std::forward<UTypes>(u).
    Remarks: The expression inside explicit is equivalent to:
            !conjunction_v<is_convertible<UTypes, Types>...>
    @fn template<class... UTypes>
        constexpr explicit(see below) tuple(UTypes&&...); // only if sizeof...(Types) >= 1
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
    4)
    Effects: Initializes each element of *this with the corresponding element of u.
    @fn tuple(const tuple&) = default;
    */
    tuple(const tuple&) requires requires {
        requires (std::copy_constructible<Types> && ...);
    } = default;

    /*
    § 20.5.3.1 
    5)  
    Effects: For all i, initializes the ith element of *this with std::forward<Ti>(get<i>(u))
    @fn tuple(tuple&&) = default;
    */
    tuple(tuple&&) requires requires {
        requires (std::move_constructible<Types> && ...);
    } = default;

    /*
    § 20.5.3.1 
    6) 
    Effects: Initializes each element of *this with the corresponding element of u.
    Remarks: The expression inside explicit is equivalent to:
            !conjunction_v<is_convertible<const UTypes&, Types>...>
    @fn template<class... UTypes> 
        constexpr explicit(see below) tuple(const tuple<UTypes...>& u);
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

    /*
    § 20.5.3.1 
    6) 
    Effects: Initializes each element of *this with the corresponding element of u.
    Remarks: The expression inside explicit is equivalent to:
            !conjunction_v<is_convertible<const UTypes&, Types>...>
    @fn template<class... UTypes> 
        constexpr explicit(see below) tuple(const tuple<UTypes...>& u);
    */
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
    7)  
    Effects: For all i, initializes the ith element of *this with std::forward<Ui>(get<i>(u)).
    Remarks: The expression inside explicit is equivalent to:
            !conjunction_v<is_convertible<UTypes, Types>...>
    @fn template<class... UTypes> 
        constexpr explicit(see below) tuple(tuple<UTypes...>&& u);
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
    : _impl{v} {}

#if MINPP_STD_COMPAT

    /*
    § 20.5.3.1 
    7) [std]
    Effects: For all i, initializes the ith element of *this with std::forward<Ui>(get<i>(u)).
    Remarks: The expression inside explicit is equivalent to:
            !conjunction_v<is_convertible<UTypes, Types>...>
    @fn template<class... UTypes> 
        constexpr explicit(see below) tuple(tuple<UTypes...>&& u);
    */
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
    : _impl{v} {}

#endif

    /*
    § 20.5.3.1 
    8)    
    Effects: Initializes the first element with u.first and the second element with u.second.
    Remarks: The expression inside explicit is equivalent to:
            !is_convertible_v<const U1&, T0> || !is_convertible_v<const U2&, T1>
    @fn template<class U1, class U2> 
        constexpr explicit(see below) tuple(const pair<U1, U2>& u); // only if sizeof...(Types) == 2
    */
    template <typename... UTypes>
    constexpr explicit(!(std::is_convertible_v<const UTypes&, Types> && ...)) tuple(const std::pair<UTypes...>& v) requires requires {
        requires sizeof...(Types) == 2;
        requires (std::constructible_from<Types, const UTypes&> && ...);
    }
    : _impl{v} {}

    /*
    § 20.5.3.1 
    9)  
    Effects: Initializes the first element with std::forward<U1>(u.first) and the second element with
        std::forward<U2>(u.second).
    Remarks: The expression inside explicit is equivalent to:
            !is_convertible_v<U1, T0> || !is_convertible_v<U2, T1>
    @fn template<class U1, class U2> 
        constexpr explicit(see below) tuple(pair<U1, U2>&& u); // only if sizeof...(Types) == 2
    */
    template <typename... UTypes>
    constexpr explicit(!(std::is_convertible_v<const UTypes&, Types> && ...)) tuple(std::pair<UTypes...>&& v) requires requires {
        requires sizeof...(Types) == 2;
        requires (std::constructible_from<Types, UTypes> && ...);
    }
    : _impl{v} {}

    // not annotated

    constexpr tuple& operator=(const tuple& other) noexcept((std::is_nothrow_copy_assignable_v<Types> && ...)) {
        _impl::operator=(other);
        return *this;
    }

    constexpr tuple& operator=(tuple&& other) noexcept((std::is_nothrow_move_assignable_v<Types> && ...)) {
        _impl::operator=(std::move(other));
        return *this;
    }

    constexpr void swap(tuple& other) noexcept((std::is_nothrow_swappable_v<Types> && ...)) {
        _impl::swap(other);
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

    constexpr void swap(tuple&) noexcept {}
};

template <typename T>
struct enable_expand_size_with_template_args: public std::false_type {};

template <template <typename...> typename _T, typename... _Us> requires requires {
    requires sizeof...(_Us) == std::tuple_size<_T<_Us...>>::value;
}
struct enable_expand_size_with_template_args<_T<_Us...>>: public std::true_type {};

template <typename T>
static constexpr bool enable_expand_size_with_template_args_v = enable_expand_size_with_template_args<T>::value;

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

/*
§ 20.5.3.4 
@fn template<class... TTypes>
constexpr tuple<unwrap_ref_decay_t<TTypes>...> make_tuple(TTypes&&... t);
@returns tuple<unwrap_ref_decay_t<TTypes>...>(std::forward<TTypes>(t)...).
*/
template <typename... TTypes>
constexpr tuple<std::unwrap_ref_decay_t<TTypes>...> make_tuple(TTypes&&... t) {
    return {std::forward<TTypes>(t)...};
}

/*
§ 20.5.3.4 
Effects: Constructs a tuple of references to the arguments in t suitable for forwarding as arguments to 
    a function. Because the result may contain references to temporary objects, a program shall ensure 
    that the return value of this function does not outlive any of its arguments (e.g., the program should 
    typically not store the result in a named variable).
@fn template<class... TTypes>
constexpr tuple<TTypes&&...> forward_as_tuple(TTypes&&... t) noexcept;
@returns tuple<TTypes&&...>(std::forward<TTypes>(t)...)
*/
template<class... TTypes>
constexpr tuple<TTypes&&...> forward_as_tuple(TTypes&&... t) noexcept {
    return {std::forward<TTypes>(t)...};
}

/*
§ 20.5.3.4 
@var inline constexpr unspecified ignore;
*/
inline constexpr impl::ignore_t ignore;

/*
§ 20.5.3.4 
@fn template<class... TTypes>
constexpr tuple<TTypes&...> tie(TTypes&... t) noexcept;
@returns tuple<TTypes&...>(t...). When an argument in t is ignore, assigning any value to the
    corresponding tuple element has no effect.
*/
template<class... TTypes>
constexpr tuple<TTypes&...> tie(TTypes&... t) noexcept {
    return {t...};
}

MINPP_NAMESPACE_END

MINPP_IMPL_BEGIN

template <typename... Ts>
struct _cat_indices {
    template <std::size_t OuterIndex, typename Inner>
    struct _make_outer_index {
        using type = make_index_duplicates<OuterIndex, std::tuple_size<Inner>::value>;
    };

    template <std::size_t OuterIndex, template <typename...> typename _InnerT, typename... _InnerUs> requires requires {
        requires enable_expand_size_with_template_args_v<_InnerT<_InnerUs...>>;
    }
    struct _make_outer_index<OuterIndex, _InnerT<_InnerUs...>> {
        using type = std::index_sequence<(std::void_t<_InnerUs>(), OuterIndex)...>;
    };

    template <std::size_t OuterIndex, typename Inner>
    using _make_outer_index_t = typename _make_outer_index<OuterIndex, Inner>::type;

    template <typename InnerCountsSequence>
    struct _inner_indices {};
    template <typename OuterIndicesSequence, typename... Inners>
    struct _outer_indices {};

    template <std::size_t... InnerCounts>
    struct _inner_indices<std::index_sequence<InnerCounts...>> {
        using type = typename flatten_value_in_integer_sequence<std::size_t>::apply_t<std::make_index_sequence<InnerCounts>...>;
    };

    template <typename... Inners, std::size_t... OuterIndices>
    struct _outer_indices<std::index_sequence<OuterIndices...>, Inners...> {
        using type = typename flatten_value_in_integer_sequence<std::size_t>::apply_t<_make_outer_index_t<OuterIndices, Inners>...>;
    };

    using _inner_indices_t = typename _inner_indices<std::index_sequence<std::tuple_size<std::remove_reference_t<Ts>>::value...>>::type;
    using _outer_indices_t = typename _outer_indices<std::index_sequence_for<Ts...>, std::remove_reference_t<Ts>...>::type;
};

template <typename... Tuples, std::size_t... Inners, std::size_t... Outers>
constexpr decltype(auto) _impl_tuple_cat(::std::index_sequence<Inners...>, ::std::index_sequence<Outers...>, Tuples&&... tpls)
{
    auto tpls_fwd = minpp::forward_as_tuple(std::forward<Tuples>(tpls)...);
    return flatten_type<tuple>::apply_t<std::remove_cvref_t<Tuples>...>{get<Inners>(get<Outers>(std::move(tpls_fwd)))...};
}

MINPP_IMPL_END

MINPP_NAMESPACE_BEGIN

/*
§ 20.5.3.4 
Remarks: The types in CTypes are equal to the ordered sequence of the extended types Args0..., 
    Args1..., . . . , Argsn−1..., where n is equal to sizeof...(Tuples). Let ei... be the ith ordered 
    sequence of tuple elements of the resulting tuple object corresponding to the type sequence Argsi.
@fn template<class... Tuples>
constexpr tuple<CTypes...> tuple_cat(Tuples&&... tpls);
@returns A tuple object constructed by initializing the kith type element eik in ei... with
        get<ki>(std::forward<Ti>(tpi))
    for each valid ki and each group ei in order.
*/
template<class... Tuples>
constexpr decltype(auto) tuple_cat(Tuples&&... tpls) {
    using cat_indices = impl::_cat_indices<Tuples...>;
    return minpp::impl::_impl_tuple_cat(typename cat_indices::_inner_indices_t{}, typename cat_indices::_outer_indices_t{}, std::forward<Tuples>(tpls)...);
}

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

template <typename... Ts>
constexpr void swap(tuple<Ts...>& lhs, tuple<Ts...>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

MINPP_NAMESPACE_END

#endif
