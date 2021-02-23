/*
steven varga, 2021, feb 22, Toronto, ON, Canada;  MIT license
*/

#include "argparse.h"
#include <h5cpp/all>
#include <string>

int main(int argc, char **argv) {
    argparse::ArgumentParser arg("ocpy", "0.0.1");
    arg.add_argument("-i", "--input").required().help("path to input hdf5 file");
    arg.add_argument("-s", "--source").default_value(std::string("/")).help("path to group within hdf5 container");
    arg.add_argument("-o", "--output").required().help("the new hdf5 will be created/or opened rw");
    arg.add_argument("-d", "--destination").default_value(std::string("/")).help("target group");
    
    std::string input, output, source, destination;
    try {
        arg.parse_args(argc, argv);
        // we are using H5CPP in mixed mode, this brings clarity, RAII, error handling mechanism, ... 
        h5::fd_t fd_i = h5::open(arg.get<std::string>("--input"), H5F_ACC_RDONLY);  // be least intrusive, most relaxed
	    h5::fd_t fd_o = h5::create(arg.get<std::string>("--output"), H5F_ACC_TRUNC);
        h5::ocpl_t ocpl = h5::expand_soft_link | h5::expand_ext_link;
        
        if(H5Ocopy(fd_i, arg.get<std::string>("--source").data(), 
            fd_o, arg.get<std::string>("--destination").data(), ocpl, h5::lcpl) < 0) throw std::runtime_error("copy error");
    } catch ( const h5::error::any& e ) {
        std::cerr << e.what() << std::endl;
        std::cout << arg;
    }
	return 0;
}