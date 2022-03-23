# hyperslab selection for oscilloscope data: possible solution
The is one below generates the datasets with the dimension you posted, and the names as well; once the datasets are ready; we are scanning the root group for all datasets with `RANK 1` as outlined in `my_callback(...)` function. Shamelessly passed `std::vector<std::string>* op_data` acts along to skim off the dataset names of interest. 
The n on it is straight forward: double IO loop pushes the data blocks of given size onto `data` accumulating all measurements we are interested. 
The verification is to print out `3.14` barely missing PI day -- Enjoy!

**note:** H5CPP RAII mechanism reduces code complexity, and increases readability, reliability...  
here is the HDF5 file, if I read you spec correctly: <a class="attachment" href="//hdf-discourse-1.s3.amazonaws.com/original/2X/7/777e8e74878c727dc51e3dd9fde377b0467c0256.h5">scope.h5</a> (20.0 KB)

```
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
```
[uploaded dataset list](https://forum.hdfgroup.org/t/read-portion-of-dataset/9545/5?u=steven)
```
ch1_samples Dataset {100000, 502/Inf}
ch2_samples Dataset {100000, 502/Inf}
ch3_samples Dataset {100000, 502/Inf}
ch4_samples Dataset {100000, 502/Inf}
ch5_samples Dataset {100000, 502/Inf}
ch6_samples Dataset {100000, 502/Inf}
ch7_samples Dataset {100000, 502/Inf}
ch8_samples Dataset {100000, 502/Inf}

/ch1_horiz_offset Dataset {100000}
/ch1_horiz_scale Dataset {100000}
/ch1_trig_offset Dataset {100000}
/ch1_trig_time Dataset {100000}
/ch1_vert_offset Dataset {100000}
/ch1_vert_scale Dataset {100000}
/ch2_horiz_offset Dataset {100000}
/ch2_horiz_scale Dataset {100000}
/ch2_trig_offset Dataset {100000}
/ch2_trig_time Dataset {100000}
/ch2_vert_offset Dataset {100000}
/ch2_vert_scale Dataset {100000}
/ch3_horiz_offset Dataset {100000}
/ch3_horiz_scale Dataset {100000}
/ch3_trig_offset Dataset {100000}
/ch3_trig_time Dataset {100000}
/ch3_vert_offset Dataset {100000}
/ch3_vert_scale Dataset {100000}
/ch4_horiz_offset Dataset {100000}
/ch4_horiz_scale Dataset {100000}
/ch4_trig_offset Dataset {100000}
/ch4_trig_time Dataset {100000}
/ch4_vert_offset Dataset {100000}
/ch4_vert_scale Dataset {100000}
/ch5_horiz_offset Dataset {100000}
/ch5_horiz_scale Dataset {100000}
/ch5_trig_offset Dataset {100000}
/ch5_trig_time Dataset {100000}
/ch5_vert_offset Dataset {100000}
/ch5_vert_scale Dataset {100000}
/ch6_horiz_offset Dataset {100000}
/ch6_horiz_scale Dataset {100000}
/ch6_trig_offset Dataset {100000}
/ch6_trig_time Dataset {100000}
/ch6_vert_offset Dataset {100000}
/ch6_vert_scale Dataset {100000}
/ch7_horiz_offset Dataset {100000}
/ch7_horiz_scale Dataset {100000}
/ch7_trig_offset Dataset {100000}
/ch7_trig_time Dataset {100000}
/ch7_vert_offset Dataset {100000}
/ch7_vert_scale Dataset {100000}
/ch8_horiz_offset Dataset {100000}
/ch8_horiz_scale Dataset {100000}
/ch8_trig_offset Dataset {100000}
/ch8_trig_time Dataset {100000}
/ch8_vert_offset Dataset {100000}
/ch8_vert_scale Dataset {100000}

}

## previous solution

# Hyperslab Selection with H5CPP
As @miller86 pointed out  hyperslab selection is the way to go. In order to make this work one has to have a datasets with chunked layout. Which is one of the possible `H5D_COMPACT | H5D_CONTIGUOUS | H5D_CHUNKED | H5D_VIRTUAL`  [dataset  creation](https://support.hdfgroup.org/HDF5/doc/RM/RM_H5P.html#Property-SetLayout) properties of the HDF5 CAPI and conveniently provided as [`h5::chunk{...}` ](http://sandbox.h5cpp.org/architecture/#dataset-operations) property list in this [C++ template only library](http://sandbox.h5cpp.org/)

![image|690x425](upload://nud5fDZASL6P5bweY63n8OAqrCz.png)

# Example
In the first part we are creating an extendable dataset with fill value `3.0` and gzip compression; and a matching dimensions; notice that armadillo is col major system whereas HDF5 os row major therefore the dimensions are flipped. 
In the second section we are to read selected region back into a `B` buffer
```
#include <armadillo>
#include <h5cpp/all>

int main(){
	h5::fd_t fd = h5::create("hyperslab.h5",H5F_ACC_TRUNC);  // and a file
	arma::mat M(10,8);                                // create a matrix
   
   { // CREATE a dataset, and write a hyperslab in it:
        h5::ds_t ds = h5::create<double>(fd,"single/chunked-dataset.mat"
                ,h5::current_dims{M.n_cols, M.n_rows}
                ,h5::max_dims{M.n_cols,H5S_UNLIMITED}
                ,h5::chunk{M.n_cols,1} | h5::fill_value<double>{3.0} |  h5::gzip{3}
        );
		arma::mat K = arma::zeros(2,3);
		h5::write(ds, K, h5::offset{2,4});
    }
	// READ selected data back into a B buffer
	arma::mat B(5,6);
	h5::read(fd,"single/chunked-dataset.mat", B, h5::offset{1,2});
	std::cout << B <<std::endl;
}
```
The printout:
```
g++-10 -I/usr/local/include -I/usr/include -I./ -o hyperslab.o  -std=c++20 -c hyperslab.cpp
g++-10 hyperslab.o -lhdf5  -lz -ldl -lm -lpthread -o hyperslab	
./hyperslab
   3.0000   3.0000   3.0000   3.0000   3.0000   3.0000
   3.0000   3.0000   3.0000   3.0000   3.0000   3.0000
   3.0000        0        0        0   3.0000   3.0000
   3.0000        0        0        0   3.0000   3.0000
   3.0000   3.0000   3.0000   3.0000   3.0000   3.0000
```

[Here is a great tutorial](https://www.hdfgroup.org/wp-content/uploads/2020/02/20200206_ECPTutorial-final.pdf) by M. Scot Breitenfeld, Elena Pourmal (The HDF Group), Suren Byna, Quincey Koziol
(Lawrence Berkeley National Laboratory) which can help you through some of the concepts.

best wishes: steven