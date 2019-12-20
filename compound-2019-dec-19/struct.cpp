/* Copyright (c) 2019 vargaconsulting, Toronto,ON Canada
 * Author: Varga, Steven <steven@vargaconsulting.ca>
 */
#include <iostream>
#include <vector>
#include "struct.h"
#include <h5cpp/core>
	// generated file must be sandwiched between core and io 
	// to satisfy template dependencies in <h5cpp/io>  
	#include "generated.h"
#include <h5cpp/io>


int main(){
	h5::fd_t fd = h5::create("test.h5", H5F_ACC_TRUNC);
	{ // this is to create the dataset
		h5::create<sn::record_t>(fd, "/Module/g_data", h5::max_dims{70,3,3} );
		// chunk must be set for partial access (huge files):  h5::chunk{1,3,3}
	}

	{ // read entire dataset back
		using T = std::vector<sn::record_t>;
		// for partial read be certain dataset is chunked, see documentation @ sandbox.h5cpp.org
		auto dataset = h5::read<T>(fd,"/Module/g_data");

		for( auto rec:dataset ) // this is your HPC loop
			std::cerr << rec.temp <<" ";
		std::cerr << std::endl;
	}
}
