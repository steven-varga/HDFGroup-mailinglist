
/*
 * Copyright (c) 2018 - 2021 vargaconsulting, Toronto,ON Canada
 * Author: Varga, Steven <steven@vargaconsulting.ca>
 */
#ifndef  H5CPP_META123_HPP 
#define  H5CPP_META123_HPP

#include "H5Iall.hpp"
#include "H5meta.hpp"
#include <type_traits>
#include <string>
#include <string_view>
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
#include <tuple>
#include <complex>

//FIXME: move it elsewhere
#define H5CPP_supported_elementary_types "supported elementary types ::= pod_struct | float | double |  [signed](int8 | int16 | int32 | int64)"

// stl detection with templates, this probably should stay until concepts become mainstream
namespace h5::meta {
    template <class... Ts> struct is_tuple : public std::false_type{};
    template <class... Ts> struct is_tuple <std::tuple<Ts...>>: public std::true_type{};


    template <class T, class... Ts> struct is_array : public std::is_array<T>{};
    template <class T, size_t N> struct is_array <std::array<T,N>> : std::true_type{};

    template<class T> using is_stl = /* will use concepts once becomes available */
        std::disjunction<meta::has_iterator<T>, meta::has_data<T>,meta::has_size<T>,meta::has_value_type<T>>;

    template <class... Args> using enable_or = std::enable_if<std::disjunction_v<Args...>>;
    template <class... Args> using enable_and = std::enable_if<std::conjunction_v<Args...>>;
    template <class T, class... Ts> struct decay {
        using type = typename meta::value<T>::type; };
    template <class T, size_t N> struct decay<T[N]>{ // support for array types
        using type = typename std::remove_all_extents<T>::type; };

    /* std::array<...,R> size<T>(){} template has to compute rank R at compile 
     * time, these templates, and their respective specializations aid to accomplish that*/
    template<class T, class... Ts> struct rank : public std::integral_constant<int,
        meta::has_size<T>::value> {}; // definition
    template<class U> struct rank<U[]>  : public std::integral_constant<std::size_t, rank<U>::value + 1>{};
    template<class U, std::size_t N> struct rank<U[N]> : public std::integral_constant<std::size_t, rank<U>::value + 1>{};
    template<class U, std::size_t N> struct rank<U*[N]> : public std::integral_constant<std::size_t, rank<U*>::value>{};
    template <size_t N> struct rank<char[N]> : public std::integral_constant<int,0>{}; // character literals

    template<class T, int N, class... Ts> using is_rank = std::integral_constant<bool, rank<T, Ts...>::value == N >;
    // helpers for is_rank<T>, don't need specialization, instead define 'rank'
    template<class T, class... Ts> using is_scalar = is_rank<T,0,Ts...>; // numerical | pod 
    template<class T, class... Ts> using is_vector = is_rank<T,1,Ts...>;
    template<class T, class... Ts> using is_matrix = is_rank<T,2,Ts...>;
    template<class T, class... Ts> using is_cube   = is_rank<T,3,Ts...>;

    template <class T> struct is_string_char : public std::false_type{};
    template <> struct is_string_char<char> : public std::true_type{};
    template <> struct is_string_char<char16_t> : public std::true_type{};
    template <> struct is_string_char<char32_t> : public std::true_type{};
    template <> struct is_string_char<wchar_t> : public std::true_type{};

    template <class T> struct is_supported_string : public std::false_type{};
    template <> struct is_supported_string <char>: public std::true_type{};


    template <class T> struct is_string : public std::false_type{};
    template <class T> struct is_string <std::basic_string<T>>: public std::integral_constant<bool,is_string_char<T>::value>{};
    template <class T> struct is_string <std::basic_string_view<T>>: public std::integral_constant<bool,is_string_char<T>::value>{};


    /* Objects may reside in continuous memory region such as vectors, matrices, POD structures can be saved/loaded in a single transfer,
     * the rest needs to be handled on a member variable bases*/
    template <class T, class... Ts> struct is_contiguous : std::integral_constant<bool, std::is_pod<T>::value> {};
    template <class T, class... Ts> struct is_contiguous <std::basic_string<T,Ts...>> : std::true_type {};
    template <class T, class... Ts> struct is_contiguous <std::basic_string_view<T,Ts...>> : std::true_type {};
    //template <size_t N> struct is_contiguous <const char*[N]> : std::false_type {};

    template <class T> struct is_contiguous <std::complex<T>> : std::true_type{};
    template <class T, class... Ts> struct is_contiguous <std::vector<T,Ts...>> :
        std::integral_constant<bool, std::is_pod<T>::value>{};
    template <class T, size_t N> struct is_contiguous <std::array<T,N>> :
        std::integral_constant<bool, std::is_pod<T>::value>{};

    template <class T, class... Ts> struct is_linalg : std::false_type {};
    template <class C, class T, class... Cs> struct is_valid : std::false_type {};


    // DEFAULT CASE
    template <class T> struct rank<T*>: public std::integral_constant<size_t,1>{};
    template <class T, class... Ts>
        typename std::enable_if<!std::is_array<T>::value, const T*>::type data(const T& ref ){ return &ref; };
    template <class T, class... Ts> 
        typename std::enable_if<meta::has_size<T>::value, std::array<size_t,1>
        >::type size(const T& ref){
        return {ref.size()};
    };
    template <class T, class... Ts> 
        typename std::enable_if<meta::is_scalar<T>::value, std::array<size_t,1>
        >::type size(const T& ref){
        return {1};
    };
    template <class T, size_t N>
        std::array<size_t,1> size(const T(&ref)[N]){ return {N};};
    template <class T, class... Ts> struct get {
        static inline T ctor( std::array<size_t,0> dims ){
            return T(); }};
    // ARRAYS


    template <class T, int N> std::array<size_t, std::rank<T[N]>::value>
        size(const T* ref ){ return  h5::meta::get_extent<T[N]>(); };
   // template <class T, int N> struct get {
   //     static inline T[N] ctor( std::array<size_t,0> dims ){
   //         return T[N](); }};


    //STD::STRING
    template<> struct rank<std::basic_string<char>>: public std::integral_constant<size_t,0>{};
    template<> struct rank<std::basic_string<wchar_t>>: public std::integral_constant<size_t,0>{};
    template<> struct rank<std::basic_string<char16_t>>: public std::integral_constant<size_t,0>{};
    template<> struct rank<std::basic_string<char32_t>>: public std::integral_constant<size_t,0>{};
    template<> struct rank<std::basic_string_view<char>>: public std::integral_constant<size_t,0>{};
    template<> struct rank<std::basic_string_view<wchar_t>>: public std::integral_constant<size_t,0>{};
    template<> struct rank<std::basic_string_view<char16_t>>: public std::integral_constant<size_t,0>{};
    template<> struct rank<std::basic_string_view<char32_t>>: public std::integral_constant<size_t,0>{};
  
   // template <class T, class... Ts> T const* data(const std::basic_string<T, Ts...>& ref ){
   //     return ref.data();
   // }
    template <class T, class... Ts> std::array<size_t,1> size( const std::basic_string<T, Ts...>& ref ){ return{ref.size()}; }
    template <class T, class... Ts> struct get<std::basic_string<T,Ts...>> {
        static inline std::basic_string<T,Ts...> ctor( std::array<size_t,1> dims ){
            return std::basic_string<T,Ts...>(); }};


    template <class T> void get_fields( T& sp ){}
    template <class T> void get_field_names( T& sp ){}
    template <class T> void get_field_attributes( T& sp ){}

// NON_CONTIGUOUS 
    template <class T> struct member {
        using type = std::tuple<void>;
        static constexpr size_t size = 0;
    };
    template <class T> using csc_t = std::tuple< //compresses sparse row: index, colptr, values
        std::vector<unsigned long>, std::vector<unsigned long>, std::vector<T>>;
    const constexpr std::tuple<const char*, const char*, const char*> 
        csc_names = {"indices", "indptr","data"};
// HID_T
//    template <class T> struct decay<h5::dt_t<T>>{ using type = T; };
}
namespace h5::impl::linalg {
    /*types accepted by BLAS/LAPACK*/
    using blas = std::tuple<float,double,std::complex<float>,std::complex<double>>;
}

namespace h5::impl {
    // what handles may have attributes:  h5::acreate, h5::aread, h5::awrite
    template <class T, class... Ts> struct has_attribute : std::false_type {};
    template <> struct has_attribute<h5::gr_t> : std::true_type {};
    template <> struct has_attribute<h5::ds_t> : std::true_type {};
    template <> struct has_attribute<h5::ob_t> : std::true_type {};
//    template <class T> struct has_attribute<h5::dt_t<T>> : std::true_type {};

    template <class T, class... Ts> struct is_location : std::false_type {};
    template <> struct is_location<h5::gr_t> : std::true_type {};
    template <> struct is_location<h5::fd_t> : std::true_type {};

    //template <class T> struct 
}

#endif
