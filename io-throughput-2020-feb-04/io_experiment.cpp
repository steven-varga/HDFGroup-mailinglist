#include <iostream>
#include "hdf5.h"
#include <armadillo>
using namespace arma;

int main()
{
    Mat<int> DATA;
    int k = 0;
    hid_t file_id, trc_dataspace_id, trc_dataset_id;
    hsize_t nRow = 601,
            nCol = 110;
    hsize_t trc_dims[2] = {nCol, nRow};
    int N = 4000; // number of datasets (400 is equal about 100 Mb, 4000 is equal about 1 Gb)

    DATA.set_size(nRow, nCol); // creating data to write of size {nRow, nCol}
    for (hsize_t i = 0; i < nRow; i++){
        for (hsize_t j = 0; j < nCol; j++){
            DATA(i,j) = k;
            k++;
        }
    }

    file_id = H5Fcreate("my_test_data.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    trc_dataspace_id = H5Screate_simple(2, trc_dims, nullptr);

    wall_clock timer;
    timer.tic();
    for(int i = 0; i < N; i++){
        std::string datasetName = std::to_string(i);
        trc_dataset_id = H5Dcreate(file_id, datasetName.c_str(), H5T_NATIVE_INT, trc_dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        H5Dwrite(trc_dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, DATA.mem);
        H5Dclose(trc_dataset_id);
    }
    H5Sclose(trc_dataspace_id);
    H5Fclose(file_id);
    double t = timer.toc();
    std::cout << "elapsed time (sec):\t" << t << std::endl;
}
