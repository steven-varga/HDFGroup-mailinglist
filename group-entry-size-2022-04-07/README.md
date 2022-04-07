# group entry size

[question][1]:
We want to use HDF5 as file format for a scientific device which records data. Besides the data, we want to store all the necessary meta data of a file, including program configurations. Our idea was to serialize our object tree, where every object has its members as subgroups. Elementary members like ints or doubles would also be a group with their value being stored as attribute inside that group.

We found that a group has quite a memory overhead (was it 2 or 20 kb?). As our object tree has several thousand members, this is problematic - a file which should be 5 Mb gets blown up to 120 Mb. Is there a way to reduce this overhead? Would compression via hdf5 help here?


```c++
#include "include/h5cpp/all"

#include <string>
#include <iterator>
#include <filesystem>

int main(int argc, char **argv) {
    std::string path = "groups.h5";
    size_t N = 1'000'000, strings_size=0;
    auto names = h5::utils::get_test_data<std::string>(N);
    for(auto a: names) strings_size+= a.size() * sizeof(std::string::value_type);

    { // code block will enforce RAII, as we need file closed to measure size
    h5::fd_t fd = h5::create(path, H5F_ACC_TRUNC);
    std::vector<h5::gr_t> gr(N);

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
```

I found the avg size to be **800 bytes** per entry:
```
g++ -I./include -o group-test.o   -std=c++17 -DFMT_HEADER_ONLY -c group-test.cpp
g++ group-test.o -lhdf5  -lz -ldl -lm  -o group-test
./group-test
79447104-1749862=77MB

avg: 776bytes/group entry
```

[1]: https://forum.hdfgroup.org/t/group-overhead-file-size-problem-for-hierarchical-data/9640
