/* Copyright (c) 2020 vargaconsulting, Toronto,ON Canada
 * Author: Varga, Steven <steven@vargaconsulting.ca>
 */
#include <iostream>
#include <armadillo>
#include <string>
#include <h5cpp/all>
#include <cstdint>

#define ITERATION 4000
#define n_rows 601
#define n_cols 110

int main(){
	h5::fd_t fd = h5::create("container.h5", H5F_ACC_TRUNC);
	std::vector<h5::ds_t> ds;
	std::vector<arma::Mat<int32_t>> M(ITERATION);
	size_t total_bytes = (ITERATION * n_rows * n_cols * sizeof(int32_t) ) / (1024*1024);
	{
		arma::wall_clock timer;
		std::vector<std::string> names = h5::utils::get_test_data<std::string>(ITERATION);
		timer.tic();

		for( auto const& name : names)
			ds.emplace_back( h5::create<int32_t>(fd, name, h5::max_dims{n_cols,n_rows} ));

		double dt = timer.toc();
		std::cout <<"CREATING "<< ITERATION << " h5::ds_t cost:" << dt <<" or " << ITERATION / dt <<"dataset / sec" <<std::endl;
	}
	{ // setup matrices
		arma::wall_clock timer;
		timer.tic();
		for( auto& m : M )
			m.randn(n_rows, n_cols);
		double dt = timer.toc();
		std::cout <<"GENERATING  "<< ITERATION << " random matrices,  cost:" << dt <<" or " << ITERATION / dt <<"matrix / sec" <<std::endl;
	}
	{ // setup matrices
		arma::wall_clock timer;
		timer.tic();
		for( size_t i=0; i < ITERATION; i++)
			h5::write(ds[i], M[i]);
		double dt = timer.toc();
		std::cout <<"WRITING  "<< ITERATION << " random matrices to dataset,  cost:" << dt 
			<<" or " << ITERATION / dt <<"matrix / sec" <<std::endl;
		std::cout <<"THROUHPUT: " << total_bytes / dt << "MB/s";
	}

}
