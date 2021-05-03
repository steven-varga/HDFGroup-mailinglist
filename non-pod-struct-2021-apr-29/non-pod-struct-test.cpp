#include <iostream>
#include <vector>
#include <algorithm>
#include <h5cpp/core>
    #include "non-pod-struct.hpp"
#include <h5cpp/io>
#include "h5bench.hpp"


namespace h3 = h5::bench;
h3::arg_x record_size{100, 1000, 10'000, 100'000};

int main(int argc, const char ** argv){
    size_t max_size = *std::max_element(record_size.begin(), record_size.end());
    
    h5::fd_t fd = h5::create("h5cpp.h5", H5F_ACC_TRUNC);
    h5::pt_t ds = h5::create<shim::pod_t>(fd, "with-id", h5::max_dims{H5S_UNLIMITED}, h5::chunk{4096}); 
    auto strings = h5::utils::get_test_data<std::string>(max_size);
    std::vector<shim::pod_t> data(max_size);
    for(size_t i=0; i<data.size(); i++)
        data[i].id = i, strncpy(data[i].name, strings[i].data(), shim::pod_t::max_lenght::value);
    // compute data transfer size:
    std::vector<size_t> transfer_size;
    for(auto i: record_size)
        transfer_size.push_back(i*sizeof(shim::pod_t));
    // actual measurement with burn in phase
    h3::throughput(
        h3::name{"bench_name"}, record_size, h3::warmup{3}, h3::sample{100}, 
    [&](size_t idx, size_t size) -> long double {
        for(size_t k=0; k<size; k++)
            h5::append(ds, data[k]);
        return transfer_size[idx];
    });
}