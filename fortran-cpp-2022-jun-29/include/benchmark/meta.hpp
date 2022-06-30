/* Copyright (c) 2019 vargaconsulting, Toronto,ON Canada
 * Author: Varga, Steven <steven@vargaconsulting.ca>
 */
#ifndef  BENCHMARK_META_HPP
#define  BENCHMARK_META_HPP

#include <type_traits>
#include <optional>
#include <stddef.h>
#include <utility>

namespace h5::meta::impl::compat {
// N4436 and https://en.cppreference.com/w/cpp/experimental/is_detected 

	struct nonesuch {
		nonesuch( ) = delete;
		//~nonesuch( ) = delete;
		nonesuch( nonesuch const& ) = delete;
		void operator = ( nonesuch const& ) = delete;
	};

	template< class... > using void_t = void;
	namespace detail {
		template <class Default, class AlwaysVoid,
				  template<class...> class Op, class... Args>
		struct detector {
		  using value_t = std::false_type;
		  using type = Default;
		};

		template <class Default, template<class...> class Op, class... Args>
		struct detector<Default, std::void_t<Op<Args...>>, Op, Args...> {
		  using value_t = std::true_type;
		  using type = Op<Args...>;
		};
	} // namespace detail

	template <template<class...> class Op, class... Args>
	using is_detected = typename detail::detector<nonesuch, void, Op, Args...>::value_t;

	template <template<class...> class Op, class... Args>
	using detected_t = typename detail::detector<nonesuch, void, Op, Args...>::type;

	template <class Default, template<class...> class Op, class... Args>
	using detected_or = detail::detector<Default, void, Op, Args...>;

	// helper templates
	template< template<class...> class Op, class... Args >
	constexpr bool is_detected_v = is_detected<Op, Args...>::value;
	template< class Default, template<class...> class Op, class... Args >
	using detected_or_t = typename detected_or<Default, Op, Args...>::type;
	template <class Expected, template<class...> class Op, class... Args>
	using is_detected_exact = std::is_same<Expected, detected_t<Op, Args...>>;
	template <class Expected, template<class...> class Op, class... Args>
	constexpr bool is_detected_exact_v = is_detected_exact<Expected, Op, Args...>::value;
	template <class To, template<class...> class Op, class... Args>
	using is_detected_convertible = std::is_convertible<detected_t<Op, Args...>, To>;
	template <class To, template<class...> class Op, class... Args>
	constexpr bool is_detected_convertible_v = is_detected_convertible<To, Op, Args...>::value;
}

namespace h5::meta::impl::detail {
	template <size_t S,  class callable_t, size_t ...Is>
	constexpr void static_for( callable_t&& callback, std::integer_sequence<size_t, Is...> ){
		( callback( std::integral_constant<size_t, S + Is>{ } ),... );
	}
}
namespace h5::meta::impl {
	// tuple_t -- tuple we are to iterate through
	// S -- start value
	// callable_t -- Callable object
	template <class tuple_t, size_t S = 0, class callable_t >
	constexpr void static_for( callable_t&& callback ){
		constexpr size_t N = std::tuple_size<tuple_t>::value;
		detail::static_for<S, callable_t>(
				std::forward<callable_t>(callback),
				std::make_integer_sequence<size_t, N - S>{ } );
	}
}

namespace h5::meta::impl { // feature detection
	using h5::meta::impl::compat::is_detected;
	template<class T> using value_t = typename T::value_type;
	template<class T> using get_value_type = compat::detected_or<compat::nonesuch, value_t, T>;
	template<class T> using has_value_type = is_detected<value_t, T>;
	// default case
	template <class T, class TagOrClass, class E = void> struct is_value_type {
		constexpr static bool value = false;
	};
	// templated classes are tagged for identification 
	template <class T, class TagOrClass> struct is_value_type <T, TagOrClass,
		typename std::enable_if<has_value_type<T>::value>::type > {
		constexpr static bool value = std::is_convertible<typename impl::get_value_type<T>::type, TagOrClass>::value;
	};
	// enum classes can't be tagged, nor pods may need tagging
	template <class T, class TagOrClass> struct is_value_type <T, TagOrClass,
		typename std::enable_if<!has_value_type<T>::value>::type > {
		constexpr static bool value = std::is_convertible<T, TagOrClass>::value;
	};
}

namespace h5::meta::arg {
	template <class T>
	struct optional : public std::optional<T> {
		using std::optional<T>::optional;
	};
}

namespace h5::meta::arg {
	// declaration
	template<class S, class... T > struct tpos;
	namespace detail {
		// declaration
		template<class search_pattern, int position, int count, bool branch, class prev_head, class arguments> struct tuple_pos;
		// initialization case 
		template<class S, int P, int C, bool B, class not_used, class... Tail >
		struct tuple_pos<S, P,C, B, not_used, std::tuple<Tail...>>
			: tuple_pos<S, P,C, false, not_used, std::tuple<Tail...>> { };
		// recursive case 
		template<class S, int P, int C, class not_used, class Head, class... Tail >
		struct tuple_pos<S, P,C,false, not_used,  std::tuple<Head, Tail...>>
			: tuple_pos<S, P+1,C, impl::is_value_type<Head,S>::value,
			Head, std::tuple<Tail...>> { };
		// match case 
		template<class S, int P, int C, class Type, class... Tail >
		struct tuple_pos<S, P,C, true, Type, std::tuple<Tail...>>
			: std::integral_constant<int,P>{
			static constexpr int position = P;
			using type  = Type;
			static constexpr bool present = true;
		 };
		// default case
		template<class S, class H, int P, int C>
		struct tuple_pos<S, P,C, false, H, std::tuple<>>
			: std::integral_constant<int,-1> {
			using type  = bool;
			static constexpr bool present = false;
		};
	}
	template<class S, class... args_t > struct tpos
		: detail::tuple_pos<const S&, -1, 0, false, void, std::tuple<args_t...>>{ };

	/* un-tagged case
	 */
	template <class T,  class... args_t,
			 class idx_t = tpos<typename T::value_type, args_t...>>
	typename std::enable_if<impl::has_value_type<T>::value, arg::optional<T>
	>::type get( args_t... args ){
		auto tuple = std::forward_as_tuple( args... );
		if constexpr ( idx_t::present )
			return std::get<idx_t::value>( tuple );
		else
			return std::nullopt;
	}
	/*
	 **/
	template <class T,  class... args_t, class idx_t = tpos<T, args_t...>>
	constexpr typename std::enable_if<!impl::has_value_type<T>::value,
	typename idx_t::type>::type get( args_t... args ){
		auto tuple = std::forward_as_tuple( args... );
		if constexpr ( idx_t::present )
			return std::get<idx_t::value>( tuple );
		else
			return false;
	}

	/*
	 **/
	template <class T, class... args_t, class idx_t = tpos<T, args_t...>>
	typename std::enable_if<!impl::has_value_type<T>::value,
	typename idx_t::type>::type required( const char msg[3], args_t... args ) {
		auto tuple = std::forward_as_tuple( args... );
		if constexpr ( idx_t::present )
			return std::get<idx_t::value>( tuple );
		else {
			static_assert( idx_t::present );
			return false;
		}
	}

	template <int idx, class... args_t>
	typename std::tuple_element<idx, std::tuple<args_t...> >::type&
	getn(args_t&&... args ){
		auto tuple = std::forward_as_tuple(args...);
		return std::get<idx>( tuple );
	}


	template<typename T> struct fn_t;  
	template<typename R, typename ...Args> 
	struct fn_t<std::function<R(Args...)>> {
		static const size_t nargs = sizeof...(Args);
		typedef R result_type;

		template <size_t I> 
		struct arg {
			typedef typename std::tuple_element<I, std::tuple<Args...>>::type type;
		};
	};
}
#endif
