/* Copyright (c) 2020 vargaconsulting, Toronto,ON Canada
 * Author: Varga, Steven <steven@vargaconsulting.ca>
 */
#include <iostream>
#include <armadillo>
#include <string>
#include <h5cpp/all>
#include <cstdint>

#ifndef n_rows
	#define n_rows 601
#endif
#ifndef n_cols
	#define n_cols 110
#endif


int main( int argc, char* argv[] ){

	size_t N = std::stoi(argv[1]);
	h5::fd_t fd = h5::create("container.h5", H5F_ACC_TRUNC);
	std::vector<h5::ds_t> ds;
	std::vector<arma::Mat<int>> M(N);

	double single_MB = (n_rows * n_cols * sizeof(int) * 1.0 ) / (1024.0*1024.0);
	double total_MB = N * single_MB;
	std::cout << "total data: " << total_MB << "MB one dataset: " << single_MB <<"MB" << std::endl;
	{
		arma::wall_clock timer;
		std::vector<std::string> names = h5::utils::get_test_data<std::string>(N);
		timer.tic();

		for( auto const& name : names)
			ds.emplace_back( h5::create<int>(fd, name, h5::max_dims{n_cols,n_rows} ));

		double dt = timer.toc();
		std::cout <<"CREATING "<< N << " h5::ds_t cost:" << dt <<" or " << N / dt <<"dataset / sec" <<std::endl;
	}
	{ // setup matrices
		arma::wall_clock timer;
		timer.tic();
		for( auto& m : M )
			m.randn(n_rows, n_cols);
		double dt = timer.toc();
		std::cout <<"GENERATING  "<< N << " random matrices,  cost:" << dt <<" or " << N / dt <<"matrix / sec" <<std::endl;
	}
	{ // do io
		arma::wall_clock timer;
		timer.tic();
		for( size_t i=0; i < N; i++)
			h5::write(ds[i], M[i]);
		double dt = timer.toc();
		std::cout <<"WRITING  "<< N << " random matrices to dataset,  cost:" << dt 
			<<" or " << N / dt <<"matrix / sec" <<std::endl;
		std::cout <<"THROUHGPUT: " << total_MB / dt << "MB/s";
		std::cerr << total_MB / dt << std::endl;
	}

}
