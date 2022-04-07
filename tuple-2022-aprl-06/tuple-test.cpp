/* copyright steven varga, vargaconsulting 2021, june 08, Toronto, ON, Canada;  MIT license
*/
#include <iostream>
#include <h5cpp/H5Uall.hpp>
#include <h5cpp/H5cout.hpp>
#include <utils/types.hpp>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <fmt/format.h>

namespace ns = h5::test;

using element_t = std::tuple<
    bool, std::string, ns::pod_t,
    unsigned char, unsigned short, unsigned int, unsigned long long int,
    char, short, int, long long int, float, double>;

template <class T> using subset_t = std::tuple<
    std::vector<T>,std::deque <T>, std::list<T>, std::forward_list<T>,
    std::stack<T, std::deque<T>>, std::stack<T, std::vector<T>>, std::stack<T, std::list<T>>,
    std::queue<T, std::deque<T>>, std::queue<T, std::list<T>>
    // NOTE: below fail because of missing comparison / hash operators:
    // std::queue<T, std::deque<T>>, std::queue<T, std::list<T>>,
    // std::priority_queue<T, std::deque<T>>, std::priority_queue<T, std::vector<T>> >;
>;

template <class T> using stl_t = std::tuple<
    std::vector<T>, std::deque<T>, std::list<T>, std::forward_list<T>, 
    std::set<T>, std::unordered_set<T>, std::multiset<T>, std::unordered_multiset<T>,
    std::stack<T, std::deque<T>>, std::stack<T, std::vector<T>>, std::stack<T, std::list<T>>,
    std::queue<T, std::deque<T>>, std::queue<T, std::list<T>>,
    std::priority_queue<T, std::deque<T>>, std::priority_queue<T, std::vector<T>>
	>;
// cross product test between stl_t<element_t> x subset<element_t>
// such that: stl_t<T> where T := {stl_t<element_t>, element_t}

void full_cross_product(){
	constexpr size_t lower=4, upper=50, min=3, max=12;
    size_t num_tests = 0;

    h5::meta::static_for<element_t>( [&]( auto x ){
        constexpr size_t x_ = x;
        using Tx = typename std::tuple_element<x_, element_t>::type;
        using container_t = stl_t<Tx>;
        h5::meta::static_for<container_t>( [&]( auto y ){
            constexpr size_t y_ = y;
            Tx element;
            using Ty = typename std::tuple_element<y_, container_t>::type;
            using crossproduct_t = subset_t<Ty>;
            h5::meta::static_for<crossproduct_t>( [&]( auto z ){
                constexpr size_t z_ = z;
                Ty element;
                using Tz = typename std::tuple_element<z_, crossproduct_t>::type;
				// ACTUAL RANDOM DATA generated of the requested type with:
				// element value range := {lower,upper} and quantity/size := {min,max}
                auto M = h5::utils::data<crossproduct_t>::get(lower, upper, min, max);
                num_tests ++;
            }); // Tz container<A> x container<B>
        }); // Ty container x element_t 
    }); // Tx element_t
    std::cout<< std::setw(4) << num_tests << " tests has been performed\n\n";
}


// TUPLE example for COO sparse matrix representation see:
// 		https://en.wikipedia.org/wiki/Sparse_matrix#Coordinate_list_(COO)
void tuple_example(){
	constexpr size_t 
		lower=4, upper=50, // control over the range of values
	 	min=5, max=5;      // and size, fixed to a number
	
	// test against all element types:
    h5::meta::static_for<element_t>( [&]( auto x ){
        constexpr size_t x_ = x;
        using value_t = typename std::tuple_element<x_, element_t>::type;
		// sparse matrix can be represented as 2 vectors of `coordinates` and a 
		// vector of `values` fast access by columns
		using coo_t = std::tuple<
            std::vector<size_t>, std::vector<size_t>, std::vector<value_t>>;
		// request random sample data from H5CPP utils:
        coo_t sparse_matrix = h5::utils::data<coo_t>::get(lower, upper, min, max);
		// pretty print for `value_t` == `float` 
		if constexpr (std::is_same<value_t, float>::value){
			auto x = std::get<0>(sparse_matrix);
			auto y = std::get<1>(sparse_matrix);
			// sparse matrix size may be characterized by the greatest coordinates 
			// of non-zero elements:
			auto max_x = std::max_element(x.begin(), x.end());
			auto max_y = std::max_element(y.begin(), y.end());
			std::cout <<"[ sparse matrix with: " << *max_x << " x " << *max_y << "  ]" << std::endl;
			std::cout <<"     x:"<< std::get<0>(sparse_matrix) << std::endl;
			std::cout <<"     y:"<< std::get<1>(sparse_matrix) << std::endl;
			std::cout <<"values:"<< std::get<2>(sparse_matrix) << std::endl;
		}
    });
    std::cout <<"\n\n";
}

void int_unordered_map(){
    std::cout<<"\tstd::unordered_map<int, std::vector<double>>\n";
    constexpr size_t lower=2, upper=10, min=5, max=5;      
    //                         key             values
    using map_t =  std::unordered_map<int, std::vector<double>>;

    // request random sample data from H5CPP utils:
    map_t map = h5::utils::data<map_t>::get(lower, upper, min, max);
    for (const auto &p : map)
        std::cout << p.first << "=>" << p.second << '\n';
    std::cout <<"\n\n";
}

void string_map(){
    std::cout<<"\tstd::map<std::string, std::vector<std::string>>\n";
    constexpr size_t lower=2, upper=10, min=5, max=5;      
    //                         key             values
    using map_t =  std::map<std::string, std::vector<std::string>>;

    // request random sample data from H5CPP utils:
    map_t map = h5::utils::data<map_t>::get(lower, upper, min, max);
    for (const auto &p : map)
        std::cout << p.first << "=>" << p.second << '\n';
    std::cout <<"\n\n";
}




int main(int argc, char **argv) {
    namespace mock = h5::utils;
    using namespace std;
    constexpr size_t lower=2, upper=3, min=5, max=5;      

	tuple_example();
	full_cross_product();
    string_map();
    int_unordered_map();
    
	return 0;
}