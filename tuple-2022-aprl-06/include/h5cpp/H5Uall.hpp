/*
 * Copyright (c) 2018-2021 vargaconsulting, Toronto,ON Canada
 * Author: Varga, Steven <steven@vargaconsulting.ca>
 */

#ifndef  H5CPP_UTIL_HPP
#define  H5CPP_UTIL_HPP

#include "H5Tmeta.hpp"
#include <math.h>
#include <array>
#include <string>
#include <limits>
#include <type_traits>
#include <algorithm>
#include <random>
#include <string.h>

namespace h5::utils::string {
    /* literals used to generate random strings*/
    template <class T> struct literal{};
    template <> struct literal <char> {
        constexpr static char value [] = 
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    };
    template <> struct literal <wchar_t> {
        constexpr static wchar_t value [] = 
            L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    };
    template <> struct literal <char16_t> {
        constexpr static char16_t value [] = 
            u"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    };
    template <> struct literal <char32_t> {
        constexpr static char32_t value [] = 
            U"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    };

    template<class T>
    std::basic_string<T> random(size_t min, size_t max, size_t a=0, size_t b=0){
        constexpr auto alphabet = literal<T>::value;
        std::random_device rd;
        std::default_random_engine rng(rd());
        std::uniform_int_distribution<> dist(0,strlen(alphabet)-1);
        std::uniform_int_distribution<> string_length(min,max);
        std::basic_string<T> str;
        size_t N = string_length(rng);
        str.reserve(N);
        std::generate_n(std::back_inserter(str), N, [&]() {
            return alphabet[dist(rng)];
        });
        return str;
    };
}

namespace h5::utils { // SCALARS
    template <bool B> using bool_t = std::integral_constant<bool, B>;
    template <class T> using is_float = bool_t<std::is_floating_point<T>::value>;
    template <class T> using is_integral = bool_t<std::is_integral<T>::value>;
    template <class T> using is_numeric = bool_t<std::is_arithmetic<T>::value && !std::is_same<T,bool>::value 
        && !std::is_class<T>::value>;
    template <class T> using is_tuple = bool_t<meta::is_tuple<T>::value>;
    template <class T> using is_container = bool_t<!meta::is_tuple<T>::value && meta::is_stl<T>::value>;

        /** @brief base case for POD structs*/  
        template <class T, typename C=bool_t<true>> struct data {
        /** @ingroup utils
        * @brief returns default T POD or class type, paramters **min** and **max** are ignored
        * The default case provides a functional implementation as long as `T` has default constructor
        * specialize this class should you need control over the returned value 
        * @param min ignored
        * @param max ignored
        * @tparam T datatype of returned value
        * @code
        * pod_t value = h5::utils::data<pod_t>::get(10, 20);
        * @endcode  
        */

        // CONTAINERS:
        static T get(size_t l, size_t u, size_t min=5, size_t max=10) {
            using element_t = typename meta::decay<T>::type;
            std::random_device rd;
            std::default_random_engine rng(rd());
            std::uniform_int_distribution<> dist(min,max);

            size_t N = dist(rng);

            T container;
            // pre-allocate room, if allowed
            if constexpr (meta::has_reserve<T>::value) {
                container.reserve(N);
            }

            if constexpr (std::is_same<T, std::string>::value) {
                container = string::random<char>(l,u, min, max);
            } else if constexpr (meta::has_emplace_back<T>::value)
                for(size_t i=0; i<N; i++) // std::vector<>, std::deque<>, std::list<>
                    container.emplace_back(utils::data<element_t>::get(l,u, min,max));
            else if constexpr(meta::has_emplace_front<T>::value)
                for(size_t i=0; i<N; i++) // std::forward_list<>
                    container.emplace_front(utils::data<element_t>::get(l,u, min,max));
            else if constexpr(meta::has_emplace<T>::value){
                // std::stack<>, std::queue<>, std::priority_queue<>, 
                // std::map<>, std::unordered_map<>
                for(size_t i=0; i<N; i++){ 
                    element_t value = utils::data<element_t>::get(l,u, min, max);
                    container.emplace(value);
                }
            } else if constexpr(meta::is_tuple<T>::value){
                using tuple_t = T;
                meta::static_for<tuple_t>( [&]( auto i ){
                using element_t = typename std::tuple_element<i,tuple_t>::type;
                    std::get<i>( container ) = h5::utils::data<element_t>::get(l, u, min, max);
                });
            }
            return container;
        }
    };

    template <class K, class V> 
    struct data <std::pair<K,V>>{
        // K is scalar, V = {scalar, rank 1}
        static std::pair<K,V> get(size_t l, size_t u, size_t min=5, size_t max=10) {
            using key_t = typename std::remove_cv<K>::type;
            using value_t = typename std::remove_cv<V>::type;
            K key = data<key_t>::get(l,u, min,max);
            V value = data<value_t>::get(l,u, min, max);
            return std::make_pair(key,value);
        }
    };

    template <class V, size_t N> 
    struct data <std::array<V,N>>{
        static std::array<V,N> get(size_t l, size_t u, size_t min=5, size_t max=10) {
            using value_t = typename std::remove_cv<V>::type;
            std::array<value_t,N> data;
            std::generate_n(data.begin(), N, [&]() {
                return h5::utils::data<value_t>::get(l,u);
            });
            return data;
        }
    };
    template <> struct data <bool>{
        /** @ingroup utils
        * @brief returns random true | false drawn from bernaulli distribution 
        * considering the domain of `T` dataype the routine draws a value from parametrized bernaulli distribution(nom/denom),
        * with the probability of `true` given by nom/denom ratio .
        * @param nom nominator
        * @param denom denominator
        * @code
        * bool value = h5::utils::data<bool>::get(0, 1);
        * @endcode  
        */
        static bool get(size_t nom, size_t denom, size_t min=0, size_t max=0) {
            std::random_device rd;
            std::default_random_engine rng(rd());
            std::bernoulli_distribution dist( static_cast<float>(nom) / static_cast<float>(denom));
            return dist(rng);
        }
    };
    template <class T> struct data <T, is_numeric<T>>{
        /** @ingroup utils
        * @brief returns random value drawn from uniform distribution with length between **min** and **max**
        * considering the domain of `T` dataype the routine draws a value from a uniformed distribution within specified range
        * @param min lower bound of integral value
        * @param max upper bound of integral value
        * @tparam T datatype of returned value
        * @code
        * unsigned short value = h5::utils::data<unsigned short>::get(10, 20);
        * @endcode  
        */
        static T get(T min, T max, size_t a=0, size_t b=0) {
            using dist_t = typename std::conditional<std::is_integral<T>::value,
                std::uniform_int_distribution<T>, std::uniform_real_distribution<T>>::type;
            std::random_device rd;
            std::default_random_engine rng(rd());
            dist_t dist(min, max);
            return dist(rng);
        }
    };
}
#endif

