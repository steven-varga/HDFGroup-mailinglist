#include <iostream>
#include <vector>
#include <algorithm>
#include <h5bench>
#include <h5cpp/core>
#include "non-pod-struct.hpp"
#include <h5cpp/io>
#include <fmt/core.h>

namespace bh = h5::bench;
bh::arg_x record_size{10'000, 100'000};
bh::warmup warmup{0};
bh::sample sample{1};
h5::chunk chunk{4096};

std::vector<size_t> get_transfer_size(const std::vector<std::string>& strings ){
    std::vector<size_t> transfer_size;
    for (size_t i =0, j=0, N = 0; i < strings.size(); i++){
        N += strings[i].length();
        if( i == record_size[j] - 1) j++, transfer_size.push_back(N);
    }
    return transfer_size;
}

template<class T> std::vector<T> convert(const std::vector<std::string>& strings){
    return std::vector<T>();
}
template <> std::vector<char[shim::pod_t::max_lenght::value]> convert(const std::vector<std::string>& strings){
    std::vector<char[shim::pod_t::max_lenght::value]> out(strings.size());
    for (size_t i = 0; i < out.size(); i++)
        strncpy(out[i], strings[i].data(), shim::pod_t::max_lenght::value);
    return out;
}

std::vector<const char*> get_data(const std::vector<std::string>& strings){
    std::vector<const char*> data(strings.size());
    // build a array of pointers to VL strings: one level of indirection 
    for (size_t i = 0; i < data.size(); i++)
        data[i] = (char*) strings[i].data();
    return data;
}

std::vector<h5::ds_t> get_datasets(const h5::fd_t& fd, const std::string& name, h5::bench::arg_x& rs){
    std::vector<h5::ds_t> ds;

    for(size_t i=0; i< rs.rank; i++)
        ds.push_back( h5::create<std::string>(fd, fmt::format(name + "-{:010d}", rs[i]), h5::current_dims{rs[i]}, chunk));
    
    return ds;
}

int main(int argc, const char **argv){
    size_t max_size = *std::max_element(record_size.begin(), record_size.end());

    h5::fd_t fd = h5::create("h5cpp.h5", H5F_ACC_TRUNC);
    auto strings = h5::utils::get_test_data<std::string>(max_size, 10, shim::pod_t::max_lenght::value);

    // LETS PRINT PUT SOME STRINGS TO GIVE YOU THE PICTURE
    fmt::print("[{:5>}] [{:^30}] [{:6}]\n", "#", "value", "lenght");
    for(size_t i=0; i<10; i++) fmt::print("{:>2d}  {:>30}  {:>8d}\n", i, strings[i], strings[i].length());
    fmt::print("\n\n");

    { // POD: FIXED LENGTH STRING + ID
        h5::pt_t ds = h5::create<shim::pod_t>(fd, "FLstring h5::append<pod_t>", h5::max_dims{H5S_UNLIMITED}, chunk);
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
            bh::name{"FLstring h5::append<pod_t>"}, record_size, warmup, sample, ds,
            [&](size_t idx, size_t size) -> double {
                for (size_t k = 0; k < size; k++)
                    h5::append(ds, data[k]);
                return transfer_size[idx];
            });
    }

    { // VL STRING, INDEXED BY HDF5 B+TREE, h5::append<std::string>
        h5::pt_t ds = h5::create<std::string>(fd, "VLstring h5::append<std::vector<std::string>> ", h5::max_dims{H5S_UNLIMITED}, chunk);
        std::vector<size_t> transfer_size = get_transfer_size(strings);
        // actual measurement with burn in phase
        bh::throughput(
            bh::name{"VLstring h5::append<std::vector<std::string>>"}, record_size, warmup, sample,
            [&](size_t idx, size_t size) -> double {
                for (size_t i = 0; i < size; i++)
                    h5::append(ds, strings[i]);
                return transfer_size[idx];
            });
    }
    { // VL STRING, INDEXED BY HDF5 B+TREE std::vector<std::string>
        auto ds = get_datasets(fd, "VLstring h5::write<std::vector<const char*>> ", record_size);
        std::vector<const char*> data = get_data(strings);
        std::vector<size_t> transfer_size = get_transfer_size(strings);

        // actual measurement with burn in phase
        bh::throughput(
            bh::name{"VLstring h5::write<std::vector<const char*>>"}, record_size, warmup, sample,
            [&](size_t idx, size_t size) -> double {
                h5::write(ds[idx], data.data(), h5::count{size});
                return transfer_size[idx];
            });
    }
    
    { // VL STRING, INDEXED BY HDF5 B+TREE std::vector<std::string>
        auto ds = get_datasets(fd, "VLstring std::vector<std::string> ", record_size);
        std::vector<size_t> transfer_size = get_transfer_size(strings);
        // actual measurement with burn in phase
        bh::throughput(
            bh::name{"VLstring std::vector<std::string>"}, record_size, warmup, sample,
            [&](size_t idx, size_t size) -> double {
                h5::write(ds[idx], strings, h5::count{size});
                return transfer_size[idx];
            });
    }

    { // FL STRING, INDEXED BY HDF5 B+TREE std::vector<std::string>
        using FL_t = char[shim::pod_t::max_lenght::value]; // type alias

        std::vector<size_t> transfer_size;
        for (auto i : record_size)
            transfer_size.push_back(i * sizeof(FL_t));
        std::vector<FL_t> data = convert<FL_t>(strings);
        
        // modify VL type to fixed length
        h5::dt_t<FL_t> dt{H5Tcreate(H5T_STRING, sizeof(FL_t))};
        H5Tset_cset(dt, H5T_CSET_UTF8); 
        std::vector<h5::ds_t> ds;
        std::vector<h5::sp_t> file_space;
        h5::dcpl_t dcpl = chunk;

        for(size_t i=0; i < record_size.rank; i++){
            hsize_t size = record_size[i];
            auto name = fmt::format("FLstring CAPI-{:010d}", size);
            h5::sp_t file_space = H5Screate_simple(1, &size, nullptr );
            ds.push_back(h5::ds_t{
                H5Dcreate2(fd, name.data(), dt, file_space, h5::default_lcpl, dcpl, h5::default_dapl)
            });
        } 
    
        // actual measurement
        bh::throughput(
            bh::name{"FLstring CAPI"}, record_size, warmup, sample,
            [&](size_t idx, size_t size_) -> double {
                hsize_t size = size_;
                // memory space
                h5::sp_t mem_space{H5Screate_simple(1, &size, nullptr )};
                H5Sselect_all(mem_space);
                // file space
                h5::sp_t file_space{H5Dget_space(ds[idx])};
                H5Sselect_all(file_space);

                H5Dwrite( ds[idx], dt, mem_space, file_space, H5P_DEFAULT, data.data());
                return transfer_size[idx];
            });
    }
    
    { // Variable Length STRING with CAPI IO calls
        std::vector<size_t> transfer_size = get_transfer_size(strings);
        std::vector<const char*> data = get_data(strings);

        // modify VL type to fixed length
        h5::dt_t<char *> dt;
        std::vector<h5::ds_t> ds;
        h5::dcpl_t dcpl = chunk;

        for(size_t i=0; i < record_size.rank; i++){
            hsize_t size = record_size[i];
            auto name = fmt::format("VLstring CAPI-{:010d}", size);
            h5::sp_t file_space = H5Screate_simple(1, &size, nullptr );
            ds.push_back(h5::ds_t{
                H5Dcreate2(fd, name.data(), dt, file_space, h5::default_lcpl, dcpl, h5::default_dapl)
            });
        } 
    
        // actual measurement
        bh::throughput(
            bh::name{"VLstring CAPI"}, record_size, warmup, sample,
            [&](size_t idx, size_t size_) -> double {
                hsize_t size = size_;
                // memory space
                h5::sp_t mem_space{H5Screate_simple(1, &size, nullptr )};
                H5Sselect_all(mem_space);
                // file space
                h5::sp_t file_space{H5Dget_space(ds[idx])};
                H5Sselect_all(file_space);

                H5Dwrite( ds[idx], dt, mem_space, file_space, H5P_DEFAULT, data.data());
                return transfer_size[idx];
            });
    }
}

