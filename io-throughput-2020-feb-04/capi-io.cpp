#include <iostream>
#include "hdf5.h"
#include <armadillo>
#include <h5cpp/all>

#ifndef n_rows
	#define n_rows 601
#endif
#ifndef n_cols
	#define n_cols 110
#endif

int main(int argc, char* argv[] ) {

    size_t k = 0,  N = std::stoi( argv[1] ); // number of datasets (400 is equal about 100 Mb, 4000 is equal about 1 Gb)
    hid_t file_id, trc_dataspace_id;
    hsize_t trc_dims[2] = {n_cols, n_rows};
	std::vector<arma::Mat<int>> M(N);
	std::vector<hid_t> ds(N);

	double single_MB = (n_rows * n_cols * sizeof(int) * 1.0 ) / (1024.0*1024.0);
	double total_MB = N * single_MB;

    file_id = H5Fcreate("my_test_data.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    trc_dataspace_id = H5Screate_simple(2, trc_dims, nullptr);

	{ // CREATE DATASETS
		arma::wall_clock timer;
		for(int i = 0; i < N; i++){
			std::string datasetName = std::to_string(i);
			ds[i] = H5Dcreate(file_id, datasetName.c_str(), H5T_NATIVE_INT, trc_dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		}
		double dt = timer.toc();
		std::cout <<"CREATING "<< N << " h5::ds_t cost:" << dt <<" or " << N / dt <<"dataset / sec" << std::endl;
	}
	{ // setup matrices
		arma::wall_clock timer;
		timer.tic();
		for( auto& m : M )
			m.randn(n_rows, n_cols);
		double dt = timer.toc();
		std::cout <<"GENERATING  "<< N << " random matrices,  cost:" << dt <<" or " << N / dt <<"matrix / sec" <<std::endl;
	}
	{
		arma::wall_clock timer;
		timer.tic();
		for(size_t i = 0; i < N; i++)
			H5Dwrite(ds[i], H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, M[i].mem);
		double dt = timer.toc();
		std::cout <<"WRITING  "<< N << " random matrices to dataset,  cost:" << dt
			<<" or " << N / dt <<"matrix / sec" <<std::endl;
		std::cout <<"THROUGHPUT: " << total_MB / dt << "MB/s";
		std::cerr << total_MB / dt << std::endl;
	}
	{ // closing handles; h5cpp has RAII
		for(size_t i = 0; i < N; i++)
			H5Dclose(ds[i]);
		H5Sclose(trc_dataspace_id);
		H5Fclose(file_id);
	}
}
