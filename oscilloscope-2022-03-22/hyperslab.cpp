/* Copyright (c) 2022 vargaconsulting, Toronto,ON Canada */

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
		h5::write( ds,  K, h5::offset{2,4} );

    }
	//
	arma::mat B(5,6);
	h5::read(fd,"single/chunked-dataset.mat", B, h5::offset{1,2});
	std::cout << B <<std::endl;
}

