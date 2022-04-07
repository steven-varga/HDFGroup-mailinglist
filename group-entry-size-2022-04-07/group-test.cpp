/*
copyright steven varga, 2022, apr 07, Toronto, ON, Canada;  MIT license
*/

#include <h5cpp/all>

#include <string>
#include <iterator>
#include <filesystem>

int main(int argc, char **argv) {
    std::string path = "groups.h5";
    size_t N = 600'000, strings_size=0;
    auto names = h5::utils::get_test_data<std::string>(N);
    for(auto a: names) strings_size+= a.size() * sizeof(std::string::value_type);

    { // code block will enforce RAII, as we need file closed to measure size
    h5::fd_t fd = h5::create(path, H5F_ACC_TRUNC);

    h5::gr_t root{H5Gopen(fd, "/", H5P_DEFAULT)}; // using H5CPP RAII
    for(size_t n=0; n < N; n++)
        h5::gr_t{H5Gcreate(root, names[n].data(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)}; 
    } // HDF5 file is closed, grab size
    namespace fs = std::filesystem;
    size_t file_size = fs::file_size(
        fs::current_path() / fs::path(path));


    std::cout << file_size <<"-"<< strings_size <<"=" << (file_size - strings_size) / 1'000'000 << "MB\n\n";
    std::cout << "avg: " << (file_size - strings_size) / N <<"bytes/group entry\n\n";
    return 0;
}
