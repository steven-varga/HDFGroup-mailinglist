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
		h5::ds_t ds = h5::create<sn::record_t>(fd, "/path/dataset", h5::max_dims{H5S_UNLIMITED} );
		// vector of strings as attribute:
		ds["attribute"] = {"first","second","...","last"};
		
		h5::pt_t pt = ds; // convert to packet table, you could go straight from vector as well
		for(int i=0; i<3; i++)
			h5::append(pt,
			// this is your pod struct 
			sn::record_t{1.0 * i, 2.0 *i ,{1,2,3,4,5},{11,12,13,14,15}});
	}

	{ // read entire dataset back
		h5::ds_t ds = h5::open(fd, "/path/dataset");

		std::vector<std::string> attribute = h5::aread<
			std::vector<std::string>>(ds, "attribute");
		std::cout << attribute <<std::endl;
		// dump data
		for( auto rec: h5::read<std::vector<sn::record_t>>(ds, "/path/dataset")) // this is your HPC loop
			std::cerr << rec.A <<" ";
		std::cerr << std::endl;
	}
}
