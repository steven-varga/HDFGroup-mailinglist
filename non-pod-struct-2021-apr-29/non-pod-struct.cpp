#include <iostream>
#include <vector>
#include <algorithm>
#include <criterion/criterion.hpp>
#include <h5cpp/core>
    #include "non-pod-struct.hpp"
#include <h5cpp/io>


BENCHMARK(HDF5_fixed_length_string, std::size_t)
{
    SETUP_BENCHMARK(
        const auto size =  GET_ARGUMENT(0);
        h5::fd_t fd = h5::create("h5cpp.h5", H5F_ACC_TRUNC);
        h5::pt_t ds = h5::create<shim::pod_t>(fd, "with-id", h5::max_dims{H5S_UNLIMITED}); 
        auto strings = h5::utils::get_test_data<std::string>(size);
        std::vector<shim::pod_t> data(size);
        for(size_t i=0; i<size; i++)
            d = {i, strings[i]};
    )

    for(const auto&element : data)
        h5::append(ds, element);
}

INVOKE_BENCHMARK_FOR_EACH(MergeSort,
  ("/10", 10),
  ("/100", 100),
  ("/1K", 1000),
  ("/10K", 10000),
  ("/100K", 100000)
)
CRITERION_BENCHMARK_MAIN()

/*
int main(void) {
    h5::fd_t fd = h5::create("h5cpp.h5",H5F_ACC_TRUNC);
    // generate random variable length strings
    size_t size = std::accumulate(data.begin(), data.end(), 1, [&](size_t n, auto& v) -> size_t{
        return n + v.length();
    });
    std::cout << "number of strings: " << data.size() << " total size (memory): " << size*sizeof(char)/1000'000.0  << " MiB" << std::endl;
    {
        auto ds = h5::create<shim::pod_t>(fd, "with-id", h5::max_dims{H5S_UNLIMITED}); 
    }
    {
        auto ds = h5::create<non::intrusive::non_pod_t>(fd, "non-instrusive-with-id", h5::max_dims{H5S_UNLIMITED}); 
    }
    {
        auto ds = h5::create<std::string>(fd, "no-index", h5::max_dims{H5S_UNLIMITED}); 
    }


    return 0;
}
*/