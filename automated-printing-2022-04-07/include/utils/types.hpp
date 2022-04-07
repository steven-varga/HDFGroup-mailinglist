/*
 * Copyright (c) 2019 vargaconsulting, Toronto,ON Canada
 * Author: Varga, Steven <steven@vargaconsulting.ca>
 */
#ifndef  H5CPP_TYPE_HPP
#define  H5CPP_TYPE_HPP

#include <complex>
#include <array>
#include <vector>
#include <deque>
#include <forward_list>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>
#include <any>
#include <functional>
#include <armadillo>
#include <random>
#include "../linalg/all"
#include "../h5cpp/H5Tmeta.hpp"
#include "../h5cpp/H5Uall.hpp"

//#include <h5cpp/utility>

namespace h5::test {
	template <class T>
	struct separator_t : std::integral_constant<size_t,3> {
		T type;
	};

	template <class T> struct is_separator : std::false_type {};
	template <class T> struct is_separator<separator_t<T>> : std::true_type {};

    struct pod_t {
      int    a;
      float  b;
      double c;
	  friend bool operator==(const pod_t& lhs, const pod_t& rhs){ return lhs.a == rhs.a; }
	  friend bool operator!=(const pod_t& lhs, const pod_t& rhs){ return !(lhs.a == rhs.a); }
	  friend bool operator>(const pod_t& lhs, const pod_t& rhs){ return lhs.a > rhs.a; }
	  friend bool operator<(const pod_t& lhs, const pod_t& rhs){ return !(lhs.a < rhs.a); }
	  template <class T> friend typename std::enable_if<std::is_integral<T>::value||std::is_floating_point<T>::value,
	  bool>::type operator>(const pod_t& lhs, const T rhs){ return lhs.a > rhs; }
	  template <class T> friend typename std::enable_if<std::is_integral<T>::value||std::is_floating_point<T>::value,
	  bool>::type operator<(const pod_t& lhs, const T rhs){ return lhs.a < rhs; }
	  friend std::ostream& operator<<(std::ostream& os, const pod_t& rhs){
		  os << "pod_t a:" << rhs.a << " b:" << rhs.b << " c:" << rhs.c;
		return os;
	  }
	  size_t operator()(const pod_t& lhs) const {
		return std::hash<int>()(lhs.a);
      };
    };



	/*y axis are element types*/
	using numerical_t = std::tuple<
		unsigned char, unsigned short, unsigned int, unsigned long long int,
		char, short, int, long long int, float, double>;
	using other_t = std::tuple<pod_t>;
	using element_t = h5::impl::cat<numerical_t, other_t>::type;

	template <class T, size_t N=1> using array_t = std::tuple< // c like array, same memory size 
		T, T[N*N*N*N*N],    T[N*N*N][N*N],   T[N*N][N][N*N], T[N][N][N][N*N], T[N][N][N][N][N]>;

	template <class T, size_t N=1> using sequential_t = std::tuple<
		std::array<T, N>, std::array<std::vector<T>,N>,
		std::vector<T>, std::vector<std::array<T,N>>,std::vector<std::vector<T>>>;

	template <class T, class K=T, class V=T> using iterable_t = std::tuple<
		std::deque<T>, std::list<T>, std::forward_list<T>,
		std::set<T>, std::multiset<T>,
		std::map<K,V>, std::multimap<K,V>,
		std::unordered_set<T>, std::unordered_multiset<T>,
		std::unordered_map<K,V>, std::unordered_multimap<K, V>>;

	template <class T> using adaptor_t = std::tuple<
		std::stack<T, std::deque<T>>, std::stack<T, std::vector<T>>, std::stack<T, std::list<T>>,
		std::queue<T, std::deque<T>>, std::queue<T, std::list<T>>,
		std::priority_queue<T, std::deque<T>>, std::priority_queue<T, std::vector<T>>>;


	template <class T=int, class K=T, class V=T, size_t N=1>
	using stl_t = typename h5::impl::cat<
		sequential_t<T,N>, iterable_t<T,K,V>, adaptor_t<T>>::type;

	template <class T = int, size_t N = 1>
	using contigous_t = typename  h5::impl::cat<
		array_t<T,N>, sequential_t<T,N>>::type;


	template <class T>
	using linalg_t = typename h5::impl::cat<
		armadillo_t<T>,	eigen_t<T>, blitz_t<T>, blaze_t<T>, dlib_t<T>, itpp_t<T>, ublas_t<T>
		>::type;

	template <class T=int, class K=T, class V=T, size_t N=1>
	using all_t = typename h5::impl::cat<
		array_t<T,N>, stl_t<T,K,V,N>, linalg_t<T>>::type;

	template <class T>
	std::vector<std::string> h5_names(){
		std::vector<std::string> data;
		h5::meta::static_for<T>( [&]( auto i ) {
			data.push_back( h5::name<std::tuple_element_t<i,T>>::value);
		});
		return data;
	}
}

// data generator
namespace h5::utils {
template <> struct data <h5::test::pod_t>{
        /** @ingroup utils
        * @brief returns random value for pod_t::a 
        * considering the domain of `T` dataype the routine draws a value from paramterized bernaulli distribution(nom/denom),
        * with the probability of `true` given by nom/denom ratio .
        * @param nom nominator
        * @param denom denominator
        * @code
        * bool value = h5::utils::data<bool>::get(0, 1);
        * @endcode  
        */
        static h5::test::pod_t get(size_t min, size_t max,  size_t a=0, size_t b=0) {

            std::random_device rd;
            std::default_random_engine rng(rd());
			std::uniform_int_distribution<int> dist(min,max);
			h5::test::pod_t payload;
			payload.a = dist(rng);
			payload.b = dist(rng);
			payload.c = dist(rng);
			return payload;
        }
    };
	
}
namespace std {
    template<> struct hash<h5::test::pod_t> {
        std::size_t operator()(const h5::test::pod_t& pod) const noexcept {
            return std::hash<int>()(pod.a);
        }
    };
}
#endif
