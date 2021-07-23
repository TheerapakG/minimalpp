#ifndef MINPP_TUPLE_H_
#define MINPP_TUPLE_H_
#include "minpp/_minpp_macros.h"
#include "minpp/common_meta.h"
#include "minpp/common_concepts.h"

#include <concepts>
#include <functional>
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
  T value{};

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

  template <typename U>
  constexpr tuple_leaf(U&& v) requires requires {
    requires !std::is_arithmetic_v<T>;
  }: value(std::forward<U>(v)) {}

  template <typename U>
  constexpr tuple_leaf(U&& v) requires requires {
    requires std::is_arithmetic_v<T>;
  }: value{std::forward<U>(v)} {}

  template <typename U>
  constexpr tuple_leaf(_select_tuple_leaf_ctor, const tuple_leaf<I, U>& v): tuple_leaf(v.value) {}

  template <typename U>
  constexpr tuple_leaf(_select_tuple_leaf_ctor, tuple_leaf<I, U>&& v): tuple_leaf{std::move(v.value)} {}

  template <typename Alloc>
  constexpr tuple_leaf(std::allocator_arg_t, const Alloc& a) requires requires {
    !leading_allocator_constructible<T, Alloc>;
    !trailing_allocator_constructible<T, Alloc>;
  }
  : tuple_leaf{} {}

  template <typename Alloc>
  constexpr tuple_leaf(std::allocator_arg_t, const Alloc& a) requires leading_allocator_constructible<T, Alloc>
  : value(std::allocator_arg_t{}, a) {}

  template <typename Alloc>
  constexpr tuple_leaf(std::allocator_arg_t, const Alloc& a) requires trailing_allocator_constructible<T, Alloc>
  : value(a) {}

  template <typename Alloc, typename U>
  constexpr tuple_leaf(std::allocator_arg_t, const Alloc& a, U&& v) requires requires {
    !leading_allocator_constructible<T, Alloc, U>;
    !trailing_allocator_constructible<T, Alloc, U>;
    requires !std::is_arithmetic_v<T>;
  }
  : value(std::forward<U>(v)) {}

  template <typename Alloc, typename U>
  constexpr tuple_leaf(std::allocator_arg_t, const Alloc& a, U&& v) requires requires {
    !leading_allocator_constructible<T, Alloc, U>;
    !trailing_allocator_constructible<T, Alloc, U>;
    requires std::is_arithmetic_v<T>;
  }
  : value{std::forward<U>(v)} {}

  template <typename Alloc, typename U>
  constexpr tuple_leaf(std::allocator_arg_t, const Alloc& a, U&& v) requires leading_allocator_constructible<T, Alloc, U>
  : value(std::allocator_arg_t{}, a, std::forward<U>(v)) {}

  template <typename Alloc, typename U>
  constexpr tuple_leaf(std::allocator_arg_t, const Alloc& a, U&& v) requires trailing_allocator_constructible<T, Alloc, U>
  : value(std::forward<U>(v), a) {}

  template <typename Alloc, typename U>
  constexpr tuple_leaf(std::allocator_arg_t, const Alloc& a, _select_tuple_leaf_ctor, const tuple_leaf<I, U>& v): tuple_leaf{std::allocator_arg_t{}, a, v.value} {}

  template <typename Alloc, typename U>
  constexpr tuple_leaf(std::allocator_arg_t, const Alloc& a, _select_tuple_leaf_ctor, tuple_leaf<I, U>&& v): tuple_leaf{std::allocator_arg_t{}, a, std::move(v.value)} {}

  constexpr void swap(tuple_leaf& other) noexcept(std::is_nothrow_swappable_v<T>) {
    using std::swap;
    swap(value, other.value);
  }
};

template<std::size_t I>
struct tuple_t_from_size {
  template<std::size_t... Is>
  struct _tuple_t_with_size {
    template<typename... T>
    struct _tuple_t: public tuple_leaf<Is, T>... {
      // § 20.5.3.1 1)
      constexpr _tuple_t() = default;

      // § 20.5.3.1 2)
      constexpr _tuple_t(const T&... v) : tuple_leaf<Is, T>{v}... {}

      // § 20.5.3.1 3)
      template <typename... UTypes>
      constexpr _tuple_t(UTypes&&... u) requires requires {
        requires sizeof...(UTypes) == sizeof...(T);
      } : tuple_leaf<Is, T>{std::forward<UTypes>(u)}... {}

      // § 20.5.3.1 4)
      _tuple_t(const _tuple_t&) = default;

      // § 20.5.3.1 5)
      _tuple_t(_tuple_t&&) = default;

      // § 20.5.3.1 6, 8)
      template <template <typename...> typename _Tuple_Like, typename... UTypes>
      constexpr _tuple_t(const _Tuple_Like<UTypes...>& v) requires requires {
        requires !std::derived_from<_Tuple_Like<UTypes...>, _tuple_t<T...>>;
        { ((get<Is>(v), void()), ...) };
      }
      : tuple_leaf<Is, T>{get<Is>(v)}... {};

      // § 20.5.3.1 6, 8) (specialized for _tuple_t)
      template <typename... UTypes>
      constexpr _tuple_t(const _tuple_t<UTypes...>& v) : tuple_leaf<Is, T>{_select_tuple_leaf_ctor{}, v}... {};

      // § 20.5.3.1 7, 9)
      template <template <typename...> typename _Tuple_Like, typename... UTypes>
      constexpr _tuple_t(_Tuple_Like<UTypes...>&& v) requires requires {
        requires !std::derived_from<_Tuple_Like<UTypes...>, _tuple_t<T...>>;
        { ((get<Is>(std::move(v)), void()), ...) };
      }
      : tuple_leaf<Is, T>{get<Is>(std::move(v))}... {};

      // § 20.5.3.1 7, 9) (specialized for _tuple_t)
      template <typename... UTypes>
      constexpr _tuple_t(_tuple_t<UTypes...>&& v) : tuple_leaf<Is, T>{_select_tuple_leaf_ctor{}, std::move(v)}... {};

      // § 20.5.3.1 10)
      template <typename Alloc>
      constexpr _tuple_t(std::allocator_arg_t, const Alloc& a) : tuple_leaf<Is, T>{std::allocator_arg_t{}, a}... {}

      // § 20.5.3.1 11)
      template <typename Alloc>
      constexpr _tuple_t(std::allocator_arg_t, const Alloc& a, const T&... v) : tuple_leaf<Is, T>{std::allocator_arg_t{}, a, v}... {}

      // § 20.5.3.1 12)
      template <typename Alloc, typename... UTypes>
      constexpr _tuple_t(std::allocator_arg_t, const Alloc& a, UTypes&&... u) requires requires {
        requires sizeof...(UTypes) == sizeof...(T);
      } : tuple_leaf<Is, T>{std::allocator_arg_t{}, a, std::forward<UTypes>(u)}... {}

      // § 20.5.3.1 13, 15, 17)
      template <typename Alloc, template <typename...> typename _Tuple_Like, typename... UTypes>
      constexpr _tuple_t(std::allocator_arg_t, const Alloc& a, const _Tuple_Like<UTypes...>& v) requires requires {
        requires !std::derived_from<_Tuple_Like<UTypes...>, _tuple_t<T...>>;
        { ((get<Is>(v), void()), ...) };
      }
      : tuple_leaf<Is, T>{std::allocator_arg_t{}, a, get<Is>(v)}... {};

      // § 20.5.3.1 13, 15, 17) (specialized for _tuple_t)
      template <typename Alloc, typename... UTypes>
      constexpr _tuple_t(std::allocator_arg_t, const Alloc& a, const _tuple_t<UTypes...>& v) : tuple_leaf<Is, T>{std::allocator_arg_t{}, a, _select_tuple_leaf_ctor{}, v}... {};

      // § 20.5.3.1 14, 16, 18)
      template <typename Alloc, template <typename...> typename _Tuple_Like, typename... UTypes>
      constexpr _tuple_t(std::allocator_arg_t, const Alloc& a, _Tuple_Like<UTypes...>&& v) requires requires {
        requires !std::derived_from<_Tuple_Like<UTypes...>, _tuple_t<T...>>;
        { ((get<Is>(std::move(v)), void()), ...) };
      }
      : tuple_leaf<Is, T>{std::allocator_arg_t{}, a, get<Is>(std::move(v))}... {};

      // § 20.5.3.1 14, 16, 18) (specialized for _tuple_t)
      template <typename Alloc, typename... UTypes>
      constexpr _tuple_t(std::allocator_arg_t, const Alloc& a, _tuple_t<UTypes...>&& v) : tuple_leaf<Is, T>{std::allocator_arg_t{}, a, _select_tuple_leaf_ctor{}, std::move(v)}... {};


      // § 20.5.3.2
      constexpr _tuple_t& operator=(const _tuple_t& u) noexcept((std::is_nothrow_copy_assignable_v<T> && ...)) {
        (tuple_leaf<Is, T>::operator=(static_cast<const tuple_leaf<Is, T>&>(u)), ...);
        return *this;
      }

      constexpr _tuple_t& operator=(_tuple_t&& u) noexcept((std::is_nothrow_move_assignable_v<T> && ...)) {
        (tuple_leaf<Is, T>::operator=(static_cast<tuple_leaf<Is, T>&&>(u)), ...);
        return *this;
      }

      template <typename... UTypes>
      constexpr _tuple_t& operator=(const _tuple_t<UTypes...>& u) {
        (tuple_leaf<Is, T>::operator=(get<Is>(u)), ...);
        return *this;
      }

      template <typename... UTypes>
      constexpr _tuple_t& operator=(_tuple_t<UTypes...>&& u) {
        (tuple_leaf<Is, T>::operator=(get<Is>(u)), ...);
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
  
  /**
    @fn constexpr explicit(see below ) tuple();
    @brief § 20.5.3.1 1) 
    Value-initializes each element.
    @remarks The expression inside explicit evaluates to true if and only if Ti is not copy-list-initializable
      from an empty list for at least one i.
  */
  constexpr explicit(!(minpp::is_list_constructible_v<Types> && ...)) tuple() requires requires {
    requires (std::constructible_from<Types> && ...);
  } = default;

  /**
    @fn constexpr explicit(see below ) tuple(const Types&...); // only if sizeof...(Types) >= 1
    @brief § 20.5.3.1 2) 
    Initializes each element with the value of the corresponding parameter.
    @remarks The expression inside explicit is equivalent to:
      !conjunction_v<is_convertible<const Types&, Types>...>
  */
  constexpr explicit(!(std::is_convertible_v<const Types&, Types> && ...)) tuple(const Types&... v) requires requires {
    // requires sizeof...(Types) > 0; // sizeof...(Types) == 0 instantiate specialized tuple template
    requires (std::copy_constructible<Types> && ...);
  }
  : _impl{v...} {}

  /**
    @fn template<class... UTypes>
      constexpr explicit(see below) tuple(UTypes&&...); // only if sizeof...(Types) >= 1
    @brief § 20.5.3.1 3) 
    Initializes the elements in the tuple with the corresponding value in std::forward<UTypes>(u).
    @remarks The expression inside explicit is equivalent to:
      !conjunction_v<is_convertible<UTypes, Types>...>
  */
  template <typename... UTypes>
  constexpr explicit(!(std::is_convertible_v<UTypes, Types> && ...)) tuple(UTypes&&... u) requires requires {
    requires sizeof...(Types) == sizeof...(UTypes);
    // requires sizeof...(Types) > 0; // sizeof...(Types) == 0 instantiate specialized tuple template
    requires (std::constructible_from<Types, UTypes> && ...);
  }
  : _impl{std::forward<UTypes>(u)...} {}

  /**
    @fn tuple(const tuple&) = default;
    @brief § 20.5.3.1 4)
    Initializes each element of *this with the corresponding element of u.
  */
  tuple(const tuple&) requires requires {
    requires (std::copy_constructible<Types> && ...);
  } = default;

  /**
    @fn tuple(tuple&&) = default;
    @brief § 20.5.3.1 5) 
    For all i, initializes the ith element of *this with std::forward<Ti>(get<i>(u))
  */
  tuple(tuple&&) requires requires {
    requires (std::move_constructible<Types> && ...);
  } = default;

  /**
    @fn template<class... UTypes> 
      constexpr explicit(see below) tuple(const tuple<UTypes...>& u);
    @brief § 20.5.3.1 6) 
    Initializes each element of *this with the corresponding element of u.
    @remarks The expression inside explicit is equivalent to:
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

  /**
    @fn template<class... UTypes> 
      constexpr explicit(see below) tuple(const tuple<UTypes...>& u);
    @brief § 20.5.3.1 6) [std] 
    Initializes each element of *this with the corresponding element of u.
    @remarks The expression inside explicit is equivalent to:
      !conjunction_v<is_convertible<const UTypes&, Types>...>
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
      };
    };
  }
  : _impl{v} {}

#endif

  /**
    @fn template<class... UTypes> 
      constexpr explicit(see below) tuple(tuple<UTypes...>&& u);
    @brief § 20.5.3.1 7) 
    For all i, initializes the ith element of *this with std::forward<Ui>(get<i>(u)).
    @remarks The expression inside explicit is equivalent to:
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
  : _impl{std::move(v)} {}

#if MINPP_STD_COMPAT

  /**
    @fn template<class... UTypes> 
      constexpr explicit(see below) tuple(tuple<UTypes...>&& u);
    @brief § 20.5.3.1 7) [std] 
    For all i, initializes the ith element of *this with std::forward<Ui>(get<i>(u)).
    @remarks The expression inside explicit is equivalent to:
      !conjunction_v<is_convertible<UTypes, Types>...>
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
      };
    };
  }
  : _impl{std::move(v)} {}

#endif

#if MINPP_STD_COMPAT

  /**
    @fn template<class U1, class U2> 
      constexpr explicit(see below) tuple(const pair<U1, U2>& u); // only if sizeof...(Types) == 2
    @brief § 20.5.3.1 8) [std] 
    Initializes the first element with u.first and the second element with u.second.
    @remarks The expression inside explicit is equivalent to:
      !is_convertible_v<const U1&, T0> || !is_convertible_v<const U2&, T1>
  */
  template <typename... UTypes>
  constexpr explicit(!(std::is_convertible_v<const UTypes&, Types> && ...)) tuple(const std::pair<UTypes...>& v) requires requires {
    requires sizeof...(Types) == 2;
    requires (std::constructible_from<Types, const UTypes&> && ...);
  }
  : _impl{v} {}

#endif

#if MINPP_STD_COMPAT

  /**
    @fn template<class U1, class U2> 
      constexpr explicit(see below) tuple(pair<U1, U2>&& u); // only if sizeof...(Types) == 2
    @brief § 20.5.3.1 9) [std] 
    Initializes the first element with std::forward<U1>(u.first) and the second element with
    std::forward<U2>(u.second).
    @remarks The expression inside explicit is equivalent to:
      !is_convertible_v<U1, T0> || !is_convertible_v<U2, T1>
  */
  template <typename... UTypes>
  constexpr explicit(!(std::is_convertible_v<const UTypes&, Types> && ...)) tuple(std::pair<UTypes...>&& v) requires requires {
    requires sizeof...(Types) == 2;
    requires (std::constructible_from<Types, UTypes> && ...);
  }
  : _impl{std::move(v)} {}

#endif

  /**
    @fn template <class Alloc>
      constexpr explicit(see below) tuple(allocator_arg_t, const Alloc& a);
    @pre Alloc meets the Cpp17Allocator requirements (Table 36).
    @brief § 20.5.3.1 10) 
    Equivalent to the preceding constructors except that each element is constructed with usesallocator construction (20.10.7.2).
  */
  template <typename Alloc>
  constexpr explicit(!(minpp::is_list_constructible_v<Types> && ...)) tuple(std::allocator_arg_t, const Alloc& a) requires requires {
    requires (std::constructible_from<std::type_identity_t<Types>> && ...); // std::type_identity_t cuz g++ doesn't want to be nice with me
  }
  : _impl{std::allocator_arg_t{}, a} {}

  /**
    @fn template<class Alloc>
      constexpr explicit(see below) tuple(allocator_arg_t, const Alloc& a, const Types&...);
    @pre Alloc meets the Cpp17Allocator requirements (Table 36).
    @brief § 20.5.3.1 11) 
    Equivalent to the preceding constructors except that each element is constructed with usesallocator construction (20.10.7.2).
  */
  template <typename Alloc, typename... UTypes>
  constexpr explicit(!(std::is_convertible_v<const Types&, Types> && ...)) tuple(std::allocator_arg_t, const Alloc& a, const Types&... v) requires requires {
    requires (std::constructible_from<std::type_identity_t<Types>> && ...); // std::type_identity_t cuz g++ doesn't want to be nice with me
  }
  : _impl{std::allocator_arg_t{}, a, v...} {}

  /**
    @fn template<class Alloc, class... UTypes>
      constexpr explicit(see below) tuple(allocator_arg_t, const Alloc& a, UTypes&&...);
    @pre Alloc meets the Cpp17Allocator requirements (Table 36).
    @brief § 20.5.3.1 12) 
    Equivalent to the preceding constructors except that each element is constructed with usesallocator construction (20.10.7.2).
  */
  template <typename Alloc, typename... UTypes>
  constexpr explicit(!(std::is_convertible_v<UTypes, Types> && ...)) tuple(std::allocator_arg_t, const Alloc& a, UTypes&&... u) requires requires {
    requires sizeof...(Types) == sizeof...(UTypes);
    // requires sizeof...(Types) > 0; // sizeof...(Types) == 0 instantiate specialized tuple template
    requires (std::constructible_from<Types, UTypes> && ...);
  }
  : _impl{std::allocator_arg_t{}, a, std::forward<UTypes>(u)...} {}

  /**
    @fn template<class Alloc>
      constexpr tuple(allocator_arg_t, const Alloc& a, const tuple&);
    @pre Alloc meets the Cpp17Allocator requirements (Table 36).
    @brief § 20.5.3.1 13) 
    Equivalent to the preceding constructors except that each element is constructed with usesallocator construction (20.10.7.2).
  */
  template <typename Alloc>
  constexpr tuple(std::allocator_arg_t, const Alloc& a, const tuple& u) requires requires {
    requires (std::copy_constructible<std::type_identity_t<Types>> && ...); // std::type_identity_t cuz g++ doesn't want to be nice with me
  }
  : _impl{std::allocator_arg_t{}, a, u} {}

  /**
    @fn template<class Alloc>
      constexpr tuple(allocator_arg_t, const Alloc& a, tuple&&);
    @pre Alloc meets the Cpp17Allocator requirements (Table 36).
    @brief § 20.5.3.1 14) 
    Equivalent to the preceding constructors except that each element is constructed with usesallocator construction (20.10.7.2).
  */
  template <typename Alloc>
  constexpr tuple(std::allocator_arg_t, const Alloc& a, tuple&& u) requires requires {
    requires (std::move_constructible<std::type_identity_t<Types>> && ...); // std::type_identity_t cuz g++ doesn't want to be nice with me
  }
  : _impl{std::allocator_arg_t{}, a, std::move(u)} {}

  /**
    @fn template<class Alloc, class... UTypes>
      constexpr explicit(see below) tuple(allocator_arg_t, const Alloc& a, const tuple<UTypes...>&);
    @pre Alloc meets the Cpp17Allocator requirements (Table 36).
    @brief § 20.5.3.1 15)
    Equivalent to the preceding constructors except that each element is constructed with usesallocator construction (20.10.7.2).
  */
  template <typename Alloc, typename... UTypes>
  constexpr explicit(!(std::is_convertible_v<const UTypes&, Types> && ...)) tuple(std::allocator_arg_t, const Alloc& a, const tuple<UTypes...>& v) requires requires {
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
  : _impl{std::allocator_arg_t{}, a, v} {}

#if MINPP_STD_COMPAT

  /**
    @fn template<class Alloc, class... UTypes>
      constexpr explicit(see below) tuple(allocator_arg_t, const Alloc& a, const tuple<UTypes...>&);
    @pre Alloc meets the Cpp17Allocator requirements (Table 36).
    @brief § 20.5.3.1 15) [std]
    Equivalent to the preceding constructors except that each element is constructed with usesallocator construction (20.10.7.2).
  */
  template <typename Alloc, typename... UTypes>
  constexpr explicit(!(std::is_convertible_v<const UTypes&, Types> && ...)) tuple(std::allocator_arg_t, const Alloc& a, const std::tuple<UTypes...>& v) requires requires {
    requires sizeof...(Types) == sizeof...(UTypes);
    requires (std::constructible_from<Types, const UTypes&> && ...);
    requires requires {
      requires sizeof...(Types) != 1 ||
      requires {
        requires !(std::convertible_to<const tuple<UTypes>&, Types> && ...);
        requires !(std::constructible_from<Types, const tuple<UTypes>&> && ...);
      };
    };
  }
  : _impl{std::allocator_arg_t{}, a, v} {}

#endif

  /**
    @fn template<class Alloc, class... UTypes>
      constexpr explicit(see below) tuple(allocator_arg_t, const Alloc& a, tuple<UTypes...>&&);
    @pre Alloc meets the Cpp17Allocator requirements (Table 36).
    @brief § 20.5.3.1 16) 
    Equivalent to the preceding constructors except that each element is constructed with usesallocator construction (20.10.7.2).
  */
  template <typename Alloc, typename... UTypes>
  constexpr explicit(!(std::is_convertible_v<UTypes, Types> && ...)) tuple(std::allocator_arg_t, const Alloc& a, tuple<UTypes...>&& v) requires requires {
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
  : _impl{std::allocator_arg_t{}, a, std::move(v)} {}

#if MINPP_STD_COMPAT

  /**
    @fn template<class Alloc, class... UTypes>
      constexpr explicit(see below) tuple(allocator_arg_t, const Alloc& a, tuple<UTypes...>&&);
    @pre Alloc meets the Cpp17Allocator requirements (Table 36).
    @brief § 20.5.3.1 16) [std]
    Equivalent to the preceding constructors except that each element is constructed with usesallocator construction (20.10.7.2).
  */
  template <typename Alloc, typename... UTypes>
  constexpr explicit(!(std::is_convertible_v<UTypes, Types> && ...)) tuple(std::allocator_arg_t, const Alloc& a, std::tuple<UTypes...>&& v) requires requires {
    requires sizeof...(Types) == sizeof...(UTypes);
    requires (std::constructible_from<Types, UTypes> && ...);
    requires requires {
      requires sizeof...(Types) != 1 ||
      requires {
        requires !(std::convertible_to<tuple<UTypes>, Types> && ...);
        requires !(std::constructible_from<Types, tuple<UTypes>> && ...);
      };
    };
  }
  : _impl{std::allocator_arg_t{}, a, std::move(v)} {}

#endif

#if MINPP_STD_COMPAT

  /**
    @fn template<class Alloc, class U1, class U2>
      constexpr explicit(see below) tuple(allocator_arg_t, const Alloc& a, const pair<U1, U2>&);
    @pre Alloc meets the Cpp17Allocator requirements (Table 36).
    @brief § 20.5.3.1 17) [std]
    Equivalent to the preceding constructors except that each element is constructed with usesallocator construction (20.10.7.2).
  */
  template <typename Alloc, typename... UTypes>
  constexpr explicit(!(std::is_convertible_v<const UTypes&, Types> && ...)) tuple(std::allocator_arg_t, const Alloc& a, const std::pair<UTypes...>& v) requires requires {
    requires sizeof...(Types) == 2;
    requires (std::constructible_from<Types, const UTypes&> && ...);
  }
  : _impl{std::allocator_arg_t{}, a, v} {}

#endif

#if MINPP_STD_COMPAT

  /**
    @fn template<class Alloc, class U1, class U2>
      constexpr explicit(see below) tuple(allocator_arg_t, const Alloc& a, pair<U1, U2>&&);
    @pre Alloc meets the Cpp17Allocator requirements (Table 36).
    @brief § 20.5.3.1 18) [std]
    Equivalent to the preceding constructors except that each element is constructed with usesallocator construction (20.10.7.2).
  */
  template <typename Alloc, typename... UTypes>
  constexpr explicit(!(std::is_convertible_v<const UTypes&, Types> && ...)) tuple(std::allocator_arg_t, const Alloc& a, std::pair<UTypes...>&& v) requires requires {
    requires sizeof...(Types) == 2;
    requires (std::constructible_from<Types, UTypes> && ...);
  }
  : _impl{std::allocator_arg_t{}, a, std::move(v)} {}

#endif


  /**
    @fn constexpr tuple& operator=(const tuple& u);
    @returns *this.
    @brief § 20.5.3.2 
    Assigns each element of u to the corresponding element of *this. 
    @remarks This operator is defined as deleted unless is_copy_assignable_v<Ti> is true for all i.
  */
  constexpr tuple& operator=(const tuple& u) noexcept((std::is_nothrow_copy_assignable_v<Types> && ...)) requires requires {
    requires (std::assignable_from<Types&, const Types&> && ...);
  } {
    _impl::operator=(u);
    return *this;
  }

  /**
    @fn constexpr tuple& operator=(tuple&& u) noexcept(see below);
    @returns *this.
    @brief § 20.5.3.2 
    For all i, assigns std::forward<Ti>(get<i>(u)) to get<i>(*this).
    @remarks The exception specification is equivalent to the logical AND of the following expressions:
      is_nothrow_move_assignable_v<Ti>
    where Ti is the ith type in Types.
  */
  constexpr tuple& operator=(tuple&& u) noexcept((std::is_nothrow_move_assignable_v<Types> && ...)) requires requires {
    requires (std::assignable_from<Types&, Types&&> && ...);
  } {
    _impl::operator=(std::move(u));
    return *this;
  }

  /**
    @fn template<class... UTypes> constexpr tuple& operator=(const tuple<UTypes...>& u);
    @returns *this.
    @brief § 20.5.3.2 
    Assigns each element of u to the corresponding element of *this.
  */
  template <typename... UTypes> 
  constexpr tuple& operator=(const tuple<UTypes...>& u) requires requires {
    requires sizeof...(Types) == sizeof...(UTypes);
    requires (std::assignable_from<Types&, const UTypes&> && ...);
  } {
    _impl::operator=(u);
    return *this;
  }

  /**
    @fn template<class... UTypes> constexpr tuple& operator=(tuple<UTypes...>&& u);
    @returns *this.
    @brief § 20.5.3.2 
    For all i, assigns std::forward<Ui>(get<i>(u)) to get<i>(*this).
  */
  template <typename... UTypes> 
  constexpr tuple& operator=(tuple<UTypes...>&& u) requires requires {
    requires sizeof...(Types) == sizeof...(UTypes);
    requires (std::assignable_from<Types&, UTypes> && ...);
  } {
    _impl::operator=(std::move(u));
    return *this;
  }

  /**
    @fn constexpr void swap(tuple& rhs) noexcept(see below);
    @pre Each element in *this is swappable with (16.4.4.3) the corresponding element in rhs
    @throws (any) Nothing unless one of the element-wise swap calls throws an exception.
    @brief § 20.5.3.3 
    Calls swap for each element in *this and its corresponding element in rhs.
    @remarks The exception specification is equivalent to the logical AND of the following expressions:
      is_nothrow_swappable_v<Ti>
    where Ti is the ith type in Types
  */
  constexpr void swap(tuple& rhs) noexcept((std::is_nothrow_swappable_v<Types> && ...)) {
    _impl::swap(rhs);
  }

  template <std::size_t I>
  constexpr decltype(auto) operator[](std::integral_constant<std::size_t, I>) & {
    return impl::_impl_at<I>(*this);
  }

  template <std::size_t I>
  constexpr decltype(auto) operator[](std::integral_constant<std::size_t, I>) && {
    return impl::_impl_at<I>(std::forward<tuple&&>(*this));
  }

  template <std::size_t I>
  constexpr decltype(auto) operator[](std::integral_constant<std::size_t, I>) const & {
    return impl::_impl_at<I>(*this);
  }

  template <std::size_t I>
  constexpr decltype(auto) operator[](std::integral_constant<std::size_t, I>) const && {
    return impl::_impl_at<I>(std::forward<const tuple&&>(*this));
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
  template <typename Alloc> 
  constexpr tuple(std::allocator_arg_t, const Alloc&) noexcept {}
  template <typename Alloc> 
  constexpr tuple(std::allocator_arg_t, const Alloc&, const tuple&) noexcept {}
  template <typename Alloc> 
  constexpr tuple(std::allocator_arg_t, const Alloc&, tuple&&) noexcept {}
#if MINPP_STD_COMPAT
  template <typename Alloc> 
  constexpr tuple(std::allocator_arg_t, const Alloc&, const std::tuple<>&) noexcept {}
  template <typename Alloc> 
  constexpr tuple(std::allocator_arg_t, const Alloc&, std::tuple<>&&) noexcept {}
#endif

  constexpr tuple& operator=(const tuple&) noexcept = default;
  constexpr tuple& operator=(tuple&&) noexcept = default;
#if MINPP_STD_COMPAT
  constexpr tuple& operator=(const std::tuple<>&) noexcept { return *this; }
  constexpr tuple& operator=(std::tuple<>&&) noexcept { return *this; }
#endif

  constexpr void swap(tuple&) noexcept {}
};

template<typename... UTypes>
tuple(UTypes...) -> tuple<UTypes...>;
template<typename... UTypes>
tuple(tuple<UTypes...>) -> tuple<UTypes...>;
template<typename Alloc, typename... UTypes>
tuple(std::allocator_arg_t, Alloc, UTypes...) -> tuple<UTypes...>;
template<typename Alloc, typename... UTypes>
tuple(std::allocator_arg_t, Alloc, tuple<UTypes...>) -> tuple<UTypes...>;
#if MINPP_STD_COMPAT
template<class T1, class T2>
tuple(std::pair<T1, T2>) -> tuple<T1, T2>;
template<typename... UTypes>
tuple(std::tuple<UTypes...>) -> tuple<UTypes...>;
template<typename Alloc, typename T1, typename T2>
tuple(std::allocator_arg_t, Alloc, std::pair<T1, T2>) -> tuple<T1, T2>;
template<typename Alloc, typename... UTypes>
tuple(std::allocator_arg_t, Alloc, std::tuple<UTypes...>) -> tuple<UTypes...>;
#endif

template <typename T>
struct enable_expand_type_with_template_args: public std::false_type {};

template <template <typename...> typename _T, typename... _Us> requires requires {
  requires sizeof...(_Us) == std::tuple_size<_T<_Us...>>::value;
}
struct enable_expand_type_with_template_args<_T<_Us...>>: public std::true_type {};

template <typename T>
static constexpr bool enable_expand_type_with_template_args_v = enable_expand_type_with_template_args<T>::value;

MINPP_NAMESPACE_END

MINPP_IMPL_BEGIN

template <typename T, typename... Ts>
concept Once_T = requires {
  requires count_v<T, Ts...> == 1; // T have to appear in Ts exactly once
};

template <typename T, typename... Ts>
constexpr T& _impl_at_type_l(tuple<Ts...>& t) noexcept requires impl::Once_T<T, Ts...> {
  return _impl_at_type_leaf<T>(t);
}

template <typename T, typename... Ts>
constexpr const T& _impl_at_type_cl(const tuple<Ts...>& t) noexcept requires impl::Once_T<T, Ts...> {
  return _impl_at_type_leaf<T>(t);
}

template <typename T, typename... Ts>
constexpr T&& _impl_at_type_r(tuple<Ts...>&& t) noexcept requires impl::Once_T<T, Ts...> {
  return _impl_at_type_leaf<T>(std::forward<minpp::tuple<Ts...>&&>(t));
}

template <typename T, typename... Ts>
constexpr const T&& _impl_at_type_cr(const tuple<Ts...>&& t) noexcept requires impl::Once_T<T, Ts...> {
  return _impl_at_type_leaf<T>(std::forward<const minpp::tuple<Ts...>&&>(t));
}

MINPP_IMPL_END

MINPP_NAMESPACE_BEGIN

/**
  @fn template<class... TTypes>
  constexpr tuple<unwrap_ref_decay_t<TTypes>...> make_tuple(TTypes&&... t);
  @returns tuple<unwrap_ref_decay_t<TTypes>...>(std::forward<TTypes>(t)...).
  @brief § 20.5.4 
*/
template <typename... TTypes>
constexpr tuple<std::unwrap_ref_decay_t<TTypes>...> make_tuple(TTypes&&... t) {
  return {std::forward<TTypes>(t)...};
}

/**
  @fn template<class... TTypes>
  constexpr tuple<TTypes&&...> forward_as_tuple(TTypes&&... t) noexcept;
  @returns tuple<TTypes&&...>(std::forward<TTypes>(t)...)
  @brief § 20.5.4 
  Constructs a tuple of references to the arguments in t suitable for forwarding as arguments to 
  a function. Because the result may contain references to temporary objects, a program shall ensure 
  that the return value of this function does not outlive any of its arguments (e.g., the program should 
  typically not store the result in a named variable).
*/
template<class... TTypes>
constexpr tuple<TTypes&&...> forward_as_tuple(TTypes&&... t) noexcept {
  return {std::forward<TTypes>(t)...};
}

/**
  @var inline constexpr unspecified ignore;
  @brief § 20.5.4 
*/
inline constexpr impl::ignore_t ignore;

/**
  @fn template<class... TTypes>
  constexpr tuple<TTypes&...> tie(TTypes&... t) noexcept;
  @returns tuple<TTypes&...>(t...). When an argument in t is ignore, assigning any value to the
  corresponding tuple element has no effect.
  @brief § 20.5.4 
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
    requires enable_expand_type_with_template_args_v<_InnerT<_InnerUs...>>;
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

/**
  @fn template<class... Tuples>
  constexpr tuple<CTypes...> tuple_cat(Tuples&&... tpls);
  @returns A tuple object constructed by initializing the kith type element eik in ei... with
    get<ki>(std::forward<Ti>(tpi))
  for each valid ki and each group ei in order.
  @brief § 20.5.4 
  @remarks The types in CTypes are equal to the ordered sequence of the extended types Args0..., 
  Args1..., . . . , Argsn−1..., where n is equal to sizeof...(Tuples). Let ei... be the ith ordered 
  sequence of tuple elements of the resulting tuple object corresponding to the type sequence Argsi.
*/
template <typename... Tuples>
constexpr decltype(auto) tuple_cat(Tuples&&... tpls) {
  using cat_indices = impl::_cat_indices<Tuples...>;
  return minpp::impl::_impl_tuple_cat(typename cat_indices::_inner_indices_t{}, typename cat_indices::_outer_indices_t{}, std::forward<Tuples>(tpls)...);
}

MINPP_NAMESPACE_END

MINPP_IMPL_BEGIN

template <typename F, typename Tuple, std::size_t... Is>
constexpr decltype(auto) _impl_apply(F&& f, Tuple&& t, std::index_sequence<Is...>)
{
  return std::invoke(std::forward<F>(f), get<Is>(std::forward<Tuple>(t))...);
}

template <typename T, typename Tuple, std::size_t... Is>
constexpr T _impl_make_from_tuple(Tuple&& t, std::index_sequence<Is...>) requires requires {
  requires !enable_expand_type_with_template_args_v<Tuple>;
  requires std::constructible_from<T, decltype(get<Is>(std::declval<Tuple>()))...>;
}
{
  return T{get<Is>(std::forward<Tuple>(t))...};
}

template <typename T, template <typename...> typename Tuple_T, typename... Ts, std::size_t... Is>
constexpr T _impl_make_from_tuple(Tuple_T<Ts...>&& t, std::index_sequence<Is...>) requires requires {
  requires enable_expand_type_with_template_args_v<Tuple_T<Ts...>>;
  requires std::constructible_from<T, Ts...>;
}
{
  return T{get<Is>(std::forward<Tuple_T<Ts...>>(t))...};
}

MINPP_IMPL_END

MINPP_NAMESPACE_BEGIN

/**
  @fn template<class F, class Tuple>
  constexpr decltype(auto) apply(F&& f, Tuple&& t);
  @brief § 20.5.5 
  Given the exposition-only function:
  @code{.cpp}
  template<class F, class Tuple, size_t... I>
  constexpr decltype(auto) apply-impl(F&& f, Tuple&& t, index_sequence<I...>) {
    // exposition only
    return INVOKE(std::forward<F>(f), std::get<I>(std::forward<Tuple>(t))...); // see 20.14.4
  }
  @endcode
  Equivalent to:
  @code{.cpp}
  return apply-impl(std::forward<F>(f), std::forward<Tuple>(t),
    make_index_sequence<tuple_size_v<remove_reference_t<Tuple>>>{});
  @endcode
*/
template <typename F, typename Tuple>
constexpr decltype(auto) apply(F&& f, Tuple&& t) {
  return impl::_impl_apply(std::forward<F>(f), std::forward<Tuple>(t), std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>{});
}

/**
  @fn template<class T, class Tuple>
  constexpr T make_from_tuple(Tuple&& t);
  @brief § 20.5.5 
  Given the exposition-only function:
  @code{.cpp}
  template<class T, class Tuple, size_t... I>
    requires is_constructible_v<T, decltype(get<I>(declval<Tuple>()))...>
  constexpr T make-from-tuple-impl(Tuple&& t, index_sequence<I...>) { // exposition only
    return T(get<I>(std::forward<Tuple>(t))...);
  }
  @endcode
  Equivalent to:
  @code{.cpp}
  return make-from-tuple-impl<T>(
    std::forward<Tuple>(t),
    make_index_sequence<tuple_size_v<remove_reference_t<Tuple>>>{});
  @endcode
  @note The type of T must be supplied as an explicit template parameter, as it cannot be deduced from the
  argument list.
*/
template <typename T, typename Tuple>
constexpr T make_from_tuple(Tuple&& t) {
  return impl::_impl_make_from_tuple<T>(std::forward<Tuple>(t), std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>{});
}

MINPP_NAMESPACE_END

MINPP_STD_BEGIN

/**
  @brief § 20.5.6 
  All specializations of tuple_size meet the Cpp17UnaryTypeTrait requirements (20.15.2) with a base
  characteristic of integral_constant<size_t, N> for some N.
*/
template <typename T>
struct tuple_size;

/**
  @brief § 20.5.6
*/
template <typename... Types>
struct tuple_size<minpp::tuple<Types...>> : std::integral_constant<std::size_t, sizeof...(Types)> {};

/**
  @pre I < sizeof...(Types).
  @brief § 20.5.6
  ::type is the type of the Ith element of Types, where indexing is zero-based.
*/
template <std::size_t I, typename... T>
struct tuple_element<I, minpp::tuple<T...>> {
  using type = decltype(_impl_typeof_helper<I>(minpp::tuple<T...>{}));
};

MINPP_STD_END

MINPP_NAMESPACE_BEGIN
/**
  @fn template<size_t I, class... Types>
  constexpr tuple_element_t<I, tuple<Types...>>&
  get(tuple<Types...>& t) noexcept;
  @returns A reference to the Ith element of t, where indexing is zero-based.
  @brief § 20.5.7
  @note The reason get is a non-member function is that if this functionality had been provided as a member
  function, code where the type depended on a template parameter would have required using the template keyword.
*/
template <std::size_t I, typename... Types>
constexpr decltype(auto) get(minpp::tuple<Types...>& t) noexcept {
  return minpp::impl::_impl_at<I>(t);
}

/**
  @fn template<size_t I, class... Types>
  constexpr tuple_element_t<I, tuple<Types...>>&&
  get(tuple<Types...>&& t) noexcept;
  @returns A reference to the Ith element of t, where indexing is zero-based.
  @brief § 20.5.7
  @note If a type T in Types is some reference type X&, the return type is X&, not X&&. However, if the
  element type is a non-reference type T, the return type is T&&.
  @note The reason get is a non-member function is that if this functionality had been provided as a member
  function, code where the type depended on a template parameter would have required using the template keyword.
*/
template <std::size_t I, typename... Types>
constexpr decltype(auto) get(minpp::tuple<Types...>&& t) noexcept {
  return minpp::impl::_impl_at<I>(std::forward<minpp::tuple<Types...>&&>(t));
}

/**
  @fn template<size_t I, class... Types>
  constexpr const tuple_element_t<I, tuple<Types...>>&
  get(const tuple<Types...>& t) noexcept;
  @returns A reference to the Ith element of t, where indexing is zero-based.
  @brief § 20.5.7
  @note Constness is shallow. If a type T in Types is some reference type X&, the return type is X&,
  not const X&. However, if the element type is a non-reference type T, the return type is const T&. This is
  consistent with how constness is defined to work for non-static data members of reference type.
  @note The reason get is a non-member function is that if this functionality had been provided as a member
  function, code where the type depended on a template parameter would have required using the template keyword.
*/
template <std::size_t I, typename... Types>
constexpr decltype(auto) get(const minpp::tuple<Types...>& t) noexcept {
  return minpp::impl::_impl_at<I>(t);
}

/**
  @fn template<size_t I, class... Types>
  constexpr const tuple_element_t<I, tuple<Types...>>&&
  get(const tuple<Types...>&& t) noexcept;
  @returns A reference to the Ith element of t, where indexing is zero-based.
  @brief § 20.5.7
  @note The reason get is a non-member function is that if this functionality had been provided as a member
  function, code where the type depended on a template parameter would have required using the template keyword.
*/
template <std::size_t I, typename... Types>
constexpr decltype(auto) get(const minpp::tuple<Types...>&& t) noexcept {
  return minpp::impl::_impl_at<I>(std::forward<const minpp::tuple<Types...>&&>(t));
}

/**
  @fn template<class T, class... Types>
  constexpr T& get(tuple<Types...>& t) noexcept;
  @returns A reference to the element of t corresponding to the type T in Types.
  @pre The type T occurs exactly once in Types.
  @brief § 20.5.7
  @note The reason get is a non-member function is that if this functionality had been provided as a member
  function, code where the type depended on a template parameter would have required using the template keyword.
*/
template <typename T, typename... Types>
constexpr T& get(minpp::tuple<Types...>& t) noexcept {
  return minpp::impl::_impl_at_type_l<T>(t);
}

/**
  @fn template<class T, class... Types>
  constexpr T&& get(tuple<Types...>&& t) noexcept;
  @returns A reference to the element of t corresponding to the type T in Types.
  @pre The type T occurs exactly once in Types.
  @brief § 20.5.7
  @note The reason get is a non-member function is that if this functionality had been provided as a member
  function, code where the type depended on a template parameter would have required using the template keyword.
*/
template <typename T, typename... Types>
constexpr T&& get(minpp::tuple<Types...>&& t) noexcept {
  return minpp::impl::_impl_at_type_r<T>(std::forward<minpp::tuple<Types...>&&>(t));
}

/**
  @fn template<class T, class... Types>
  constexpr const T& get(const tuple<Types...>& t) noexcept;
  @returns A reference to the element of t corresponding to the type T in Types.
  @pre The type T occurs exactly once in Types.
  @brief § 20.5.7
  @note The reason get is a non-member function is that if this functionality had been provided as a member
  function, code where the type depended on a template parameter would have required using the template keyword.
*/
template <typename T, typename... Types>
constexpr const T& get(const minpp::tuple<Types...>& t) noexcept {
  return minpp::impl::_impl_at_type_cl<T>(t);
}

/**
  @fn template<class T, class... Types>
  constexpr const T&& get(const tuple<Types...>&& t) noexcept;
  @returns A reference to the element of t corresponding to the type T in Types.
  @pre The type T occurs exactly once in Types.
  @brief § 20.5.7
  @note The reason get is a non-member function is that if this functionality had been provided as a member
  function, code where the type depended on a template parameter would have required using the template keyword.
*/
template <typename T, typename... Types>
constexpr const T&& get(const minpp::tuple<Types...>&& t) noexcept {
  return minpp::impl::_impl_at_type_cr<T>(std::forward<const minpp::tuple<Types...>&&>(t));
}

MINPP_NAMESPACE_END

MINPP_IMPL_BEGIN

template <typename TTuple, typename UTuple, std::size_t... Is>
constexpr bool _impl_tuple_eq(const TTuple& t, const UTuple& u, std::index_sequence<Is...>) {
  return (... && (get<Is>(t) == get<Is>(u)));
}

template <typename Category, typename TTuple, typename UTuple, std::size_t... Is>
constexpr Category _impl_tuple_three_way(const TTuple& t, const UTuple& u, std::index_sequence<Is...>) {
  Category _comp_val = std::strong_ordering::equal;
  (... || ((_comp_val = _synth_three_way(get<Is>(t), get<Is>(u))) != 0));
  return _comp_val;
}

MINPP_IMPL_END

MINPP_NAMESPACE_BEGIN

/**
  @fn template<class... TTypes, class... UTypes>
  constexpr bool operator==(const tuple<TTypes...>& t, const tuple<UTypes...>& u);
  @returns true if get<i>(t) == get<i>(u) for all i, otherwise false. For any two zero-length tuples
  e and f, e == f returns true.
  @pre For all i, where 0 ≤ i < sizeof...(TTypes), get<i>(t) == get<i>(u) is a valid expression returning a type that is convertible to bool. sizeof...(TTypes) equals sizeof...(UTypes).
  @brief § 20.5.8
  @remarks The elementary comparisons are performed in order from the zeroth index upwards. No
  comparisons or element accesses are performed after the first equality comparison that evaluates to
  false.
*/
template <typename... TTypes, typename... UTypes>
constexpr bool operator==(const tuple<TTypes...>& t, const tuple<UTypes...>& u) {
  return _impl_tuple_eq(t, u, std::make_index_sequence<sizeof...(TTypes)>{});
}

/**
  @fn template<class... TTypes, class... UTypes>
  constexpr common_comparison_category_t<synth-three-way-result <TTypes, UTypes>...>
  operator<=>(const tuple<TTypes...>& t, const tuple<UTypes...>& u);
  @brief § 20.5.8 
  Performs a lexicographical comparison between t and u. For any two zero-length tuples t and
  u, t <=> u returns strong_ordering::equal. Otherwise, equivalent to:
    if (auto c = synth-three-way (get<0>(t), get<0>(u)); c != 0) return c;
    return ttail <=> utail;
  where rtail for some tuple r is a tuple containing all but the first element of r.
  @note The above definition does not require ttail (or utail) to be constructed. It might not even be possible, as t
  and u are not required to be copy constructible. Also, all comparison operator functions are short circuited; they do
  not perform element accesses beyond what is required to determine the result of the comparison.
*/
template <typename... TTypes, typename... UTypes>
constexpr auto operator<=>(const tuple<TTypes...>& t, const tuple<UTypes...>& u) {
  return _impl_tuple_three_way<std::common_comparison_category_t<_synth_three_way_result<TTypes, UTypes>...>>(t, u, std::make_index_sequence<sizeof...(TTypes)>{});
}

MINPP_NAMESPACE_END

MINPP_STD_BEGIN

/**
  @pre Alloc meets the Cpp17Allocator requirements (Table 36).
  @brief § 20.5.9 
  @note Specialization of this trait informs other library components that tuple can be constructed with an
  allocator, even though it does not have a nested allocator_type.
*/
template <typename... Types, typename Alloc>
struct uses_allocator<minpp::tuple<Types...>, Alloc> : std::true_type {};

MINPP_STD_END

MINPP_NAMESPACE_BEGIN

/**
  @fn template<class... Types>
  constexpr void swap(tuple<Types...>& x, tuple<Types...>& y) noexcept(see below);
  @brief § 20.5.10 
  As if by x.swap(y).
  @remarks The exception specification is equivalent to:
  noexcept(x.swap(y))
*/
template <typename... Types>
constexpr void swap(tuple<Types...>& x, tuple<Types...>& y) noexcept(noexcept(x.swap(y))) {
  x.swap(y);
}

MINPP_NAMESPACE_END

#endif
