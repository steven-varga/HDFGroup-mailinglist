/* Copyright (c) 2022 vargaconsulting, Toronto,ON Canada License: MIT */
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
		h5::create<long double>(fd,"homogenious", h5::current_dims{5,3}, h5::chunk{1,3} | h5::fill_value<long double>(-.42));
		//reading data back, then iterating through the temporary vector of LONG DOUBLES
		for (auto v:h5::read<std::vector<long double>>(fd, "homogenious"))
			std::cerr << v << " ";
		std::cerr << std::endl;
	}

	{ // an example for streams/packet table
		h5::pt_t pt = h5::create<sn::record_t>( fd, 
        	"stream-of-records", h5::max_dims{H5S_UNLIMITED}, h5::chunk{512} | h5::gzip{9} );
		sn::record_t record;	
		for(int i=0; i<10; i++ ) // this is your HPC loop
			record.temp = static_cast<long double>(i/100.0), h5::append(pt, record);
	}
	{ // read entire dataset back from tream
		using T = std::vector<sn::record_t>;
		// for partial read be certain dataset is chunked, see documentation @ sandbox.h5cpp.org
		auto dataset = h5::read<T>(fd,"stream-of-records");

		for( auto rec:dataset ) // this is your HPC loop
			std::cerr << rec.temp <<" ";
		std::cerr << std::endl;
	}


}
