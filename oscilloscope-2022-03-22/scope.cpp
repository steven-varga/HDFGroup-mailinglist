/* Copyright (c) 2022 vargaconsulting, Toronto,ON Canada */

#define FMT_HEADER_ONLY

#include <armadillo>
#include <h5cpp/all>
#include <cstddef>
#include <fmt/core.h>

// our callback funciton
herr_t my_callback(hid_t group, const char* name, const H5L_info_t* info, void* op_data){
	h5::ds_t ds{H5Dopen(group, name, H5P_DEFAULT)}; // H5CPP with CAPI
	h5::sp_t file_space{H5Dget_space(ds)}; // H5CPP RAII
	// we are not interested in other than rank 1 data (vector)
	if( H5Sget_simple_extent_ndims(file_space) != 1) return 0;
	//H5Dread(ds, mem_type, mem_space, file_space, h5::default_dxpl, payload.data());
	using T = std::vector<std::string>;
	T* names = static_cast<T*>(op_data); 
	names->push_back(name);
	return 0;
}
// 
// typedef herr_t (*H5L_iterate_t)(hid_t group, const char *name, const H5L_info_t *info, void *op_data);

int main(){
	h5::fd_t fd = h5::create("scope.h5",H5F_ACC_TRUNC);  // and a file
	// BEGIN SETUP
	size_t n=100000, m=502;
	std::vector orientation {"horiz","trig","vert"};
	std::vector type{"offset","scale"};


	for (size_t ch=1; ch<=8; ch++){
		// this one is your samples
		h5::create<double>(fd, fmt::format("ch{}_samples", ch), 
			h5::current_dims{n,m}, h5::max_dims{n, H5S_UNLIMITED}, h5::chunk{1,512});
		// and here are your datasets with fake data
		for(auto o : orientation) for(auto t:type)
			h5::create<double>(fd, fmt::format("ch{}_{}_{}", ch, o, t),
				h5::current_dims{n}, h5::chunk{512}|h5::fill_value<double>(3.14));
	}
	// END SETUP

	// SOLUTION
	std::vector<std::vector<double>> data;
	std::vector<std::string> index;
	
	// H5CPP is designed to mix CAPI code with H5CPP templates:
	h5::gr_t group_id{H5Gopen(fd, "/", 0)}; // becomes an H5CPP managed resource (RAII)
	hsize_t idx =0;
	H5Literate(group_id, H5_INDEX_NAME, H5_ITER_NATIVE, &idx, &my_callback, &index);
	// we have all the names, check it out:
	for(auto path: index)
		data.push_back( // following reads in an std::vector<double> with size h5::count{..}
			h5::read<std::vector<double>>(fd, path, h5::count{123}));
	// verification, we should see `3.14` since we just passed PI day! 
	for(auto i:data) for( auto j:i)
		std::cout << j <<" ";
	std::cout <<std::endl;

}

