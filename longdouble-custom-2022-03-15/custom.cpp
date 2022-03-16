/* Copyright (c) 2020 vargaconsulting, Toronto,ON Canada
 * Author: Varga, Steven <steven@vargaconsulting.ca>
 */

#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <algorithm>
#include <h5cpp/core> // include this before custom type definition
	#include "custom.hpp"
	#include "opaque.hpp"
#include <h5cpp/io> // IO operators become aware of your custom type


// in all cases when filtering used chunk must be set as well, no contiguous 
int main(){
	namespace cu = custom;
	h5::fd_t fd;
	try {
		h5::mute(); // turn HDF5 error handler off
			fd = h5::create("example.h5",H5F_ACC_EXCL);
		h5::unmute();
	} catch(  const h5::error::io::file::create& e  ){
		std::cout << e.what() <<std::endl;
		fd = h5::open("example.h5",H5F_ACC_RDWR);
	}
	// prints out type info, eases on debugging
	std::cout << h5::dt_t<cu::ldouble_t>() << std::endl;

	std::random_device rd;
    std::mt19937 random_float(rd());
    std::uniform_real_distribution<long double> sample(.0, 10.0);

	{ //method 3, use STL 
		std::vector<cu::ldouble_t> V(20);
		std::generate(V.begin(), V.end(), [&](){
			return static_cast<cu::ldouble_t>(sample(random_float));
		});
		for( int i=0; i<V.size(); i++ )
			std::cout << static_cast<long double>( V[i] ) << " ";
		std::cout << std::endl;

		h5::write(fd, "custom", V); // single shot write

		auto data = h5::read<std::vector<cu::ldouble_t>>(fd, "custom");
		
		for( int i=0; i<data.size(); i++ )
			std::cout << static_cast<long double>( data[i] ) << " ";
		std::cout << std::endl;

		std::cout << "\n\ncomputing difference ||saved - read|| expecting norm to be zero:\n";
		for( int i=0; i<V.size(); i++ )
			std::cout << abs(V[i].value - data[i].value) <<" ";
	}
	std::cout <<"\n\nsize of long double: " << sizeof(long double) << " sizeof ldouble_t: " << sizeof(cu::ldouble_t) <<"\n";
}

