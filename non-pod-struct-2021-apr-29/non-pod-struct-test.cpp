#include <iostream>
#include <vector>
#include <algorithm>
#include <h5bench>
#include <h5cpp/core>
#include "non-pod-struct.hpp"
#include <h5cpp/io>
#include <fmt/core.h>

namespace bh = h5::bench;
bh::arg_x record_size{10'000, 100'000, 1'000'000};
bh::warmup warmup{5};
bh::sample sample{10};
h5::chunk chunk{4096};

int main(int argc, const char **argv)
{
    size_t max_size = *std::max_element(record_size.begin(), record_size.end());

    h5::fd_t fd = h5::create("h5cpp.h5", H5F_ACC_TRUNC);
    auto strings = h5::utils::get_test_data<std::string>(max_size);

    // LETS PRINT PUT SOME STRINGS TO GIVE YOU THE PICTURE
    fmt::print("[{:5>}] [{:^30}] [{:6}]\n", "#", "value", "lenght");
    for(size_t i=0; i<10; i++) fmt::print("{:>2d}  {:>30}  {:>8d}\n", i, strings[i], strings[i].length());
    fmt::print("\n\n");

    { // POD: FIXED LENGTH STRING + ID
        h5::pt_t ds = h5::create<shim::pod_t>(fd, "with-id", h5::max_dims{H5S_UNLIMITED}, chunk);
        std::vector<shim::pod_t> data(max_size);
        // we have to copy the string into the pos struct
        for (size_t i = 0; i < data.size(); i++)
            data[i].id = i, strncpy(data[i].name, strings[i].data(), shim::pod_t::max_lenght::value);

        // compute data transfer size, we will be using this to measure throughput:
        std::vector<size_t> transfer_size;
        for (auto i : record_size)
            transfer_size.push_back(i * sizeof(shim::pod_t));

        // actual measurement with burn in phase
        bh::throughput(
            bh::name{"pod fixed length str"}, record_size, warmup, sample, ds,
            [&](size_t idx, size_t size) -> double {
                for (size_t k = 0; k < size; k++)
                    h5::append(ds, data[k]);
                return transfer_size[idx];
            });
    }

    { // VL STRING, INDEXED BY HDF5 B+TREE
        h5::pt_t ds = h5::create<std::string>(fd, "VL-string", h5::max_dims{H5S_UNLIMITED}, chunk);

        // compute data transfer size:
        std::vector<size_t> transfer_size;
        for (size_t i =0, j=0, N = 0; i < strings.size(); i++){
            N += strings[i].length();
            if( i == record_size[j] - 1) j++, transfer_size.push_back(N);
        }
        // actual measurement with burn in phase
        bh::throughput(
            bh::name{"string with VL"}, record_size, warmup, sample,
            [&](size_t idx, size_t size) -> double {
                //std::cout << idx << " " << size << " " << strings.size() <<"\n";
                for (size_t i = 0; i < size; i++)
                    h5::append(ds, strings[i]);
                return transfer_size[idx];
            });
    }
    { // VL STRING, INDEXED BY HDF5 B+TREE std::vector<std::string>
        std::vector<h5::ds_t> ds;
        for(auto s:record_size) ds.push_back( h5::create<std::string>(fd, 
            fmt::format("vector<std::string>-{:010d}", s), h5::current_dims{s}, chunk));
        std::vector<char*> data(max_size);
        // build a array of pointers to VL strings: one level of indirection 
        for (size_t i = 0; i < data.size(); i++)
            data[i] = strings[i].data();

        // compute data transfer size:
        std::vector<size_t> transfer_size;
        for (size_t i =0, j=0, N = 0; i < strings.size(); i++){
            N += strings[i].length();
            if( i == record_size[j] - 1) j++, transfer_size.push_back(N);
        }
        // actual measurement with burn in phase
        bh::throughput(
            bh::name{"string with VL block write"}, record_size, warmup, sample,
            [&](size_t idx, size_t size) -> double {
                h5::write(ds[idx], data.data(), h5::count{size});
                return transfer_size[idx];
            });
    }
}