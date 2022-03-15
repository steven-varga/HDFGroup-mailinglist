#include <vector>
#include <armadillo>
#include <h5cpp/all>

int main(void) {
    h5::fd_t fd = h5::create("h5cpp.h5",H5F_ACC_TRUNC);
    arma::mat data(10,5);
    
    { // 
    h5::ds_t ds = h5::write(fd,"some_dataset", data);  // write dataset, and obtain descriptor
    h5::awrite(ds, "attribute_name", {1,2,3,4,5,6,7});
    }
    {
        // obtain `h5::ds_t` dataset descriptor 
        // (you could use `h5::read | h5::create | h5::write` as well)
        h5::ds_t ds = h5::open(fd,"some_dataset");
        auto values = h5::aread<std::vector<int>>(ds, "attribute_name");
        
        // this is your update step
        for (int i=20; i<27; i++) values.push_back(i);
        // H5CPP doesn't provide h5::adelete, but by design you can mix C library calls:    
        H5Adelete(ds,  "attribute_name");
        h5::awrite(ds, "attribute_name", values);
    }
}
