#include <vector>
#include <armadillo>
#include <h5cpp/all>

int main(void) {
    h5::fd_t fd = h5::create("h5cpp.h5",H5F_ACC_TRUNC);
    arma::wall_clock timer;
    size_t N = 8000;
    arma::fvec data(N, arma::fill::randu);
    int I = 1;
    arma::vec datasetTime(I);
    std::vector<h5::ds_t> datasetList(I);
    h5::current_dims dims{N};
    
    for (int i = 0; i < I; i++) {
        timer.tic();
        datasetList[i] = h5::create<double>(fd, std::to_string(i), dims);
        datasetTime(i) = timer.toc();
    }
    
    arma::vec attrTime(I);
    for (int i = 0; i < I; i++) {
        timer.tic();
        h5::awrite(datasetList[i],"Attr", data);
        attrTime(i) = timer.toc();
    }
    std::cout << arma::mean(datasetTime) << std::endl;
    std::cout << arma::mean(attrTime) << std::endl;
}
