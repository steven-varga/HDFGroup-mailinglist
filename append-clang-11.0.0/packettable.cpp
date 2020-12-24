#include <h5cpp/all>
#include <filesystem>

int main() {
    h5::fd_t fd = h5::create("example.h5", H5F_ACC_TRUNC);
    std::vector<int> stream(83);
    std::iota(std::begin(stream), std::end(stream), 1);
    h5::pt_t pt = h5::create<int>(fd, "stream of integral", h5::max_dims{H5S_UNLIMITED, 3, 5},
                                  h5::chunk{2, 3, 5} | h5::gzip{9} | h5::fill_value<int>(3));
    for (auto record : stream) {
        h5::append(pt, record);
    }
}
