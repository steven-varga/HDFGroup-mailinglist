/*
steven varga, 2021, feb 22, Toronto, ON, Canada;  MIT license
*/

#include "argparse.h"
#include <h5cpp/all>
#include <string>

/* from: https://portal.hdfgroup.org/display/HDF5/H5O_VISIT1
The parameters of this callback function have the following values or meanings:

obj	 - Object that serves as root of the iteration; same value as the H5Ovisit object_id parameter
name - Name of object, relative to o_id, being examined at current step of the iteration
info - H5O_info2_t struct containing information regarding that object
op_data - User-defined pointer to data required by the application in processing the object; a pass-through of the op_data pointer provided with the H5Ovisit_by_name function call

Information struct for object (For H5Oget_info/H5Oget_info_by_name/H5Oget_info_by_idx versions 1 & 2)
typedef struct H5O_info1_t {
    unsigned long  fileno;    File number that object is located in
    haddr_t        addr;      Object address in file
    H5O_type_t     type;      H5O_TYPE_DATASET | H5O_TYPE_GROUP | H5O_TYPE_NAMED_DATATYPE |H5O_TYPE_NTYPES | H5O_TYPE_UNKNOWN 
    unsigned       rc;        Reference count of object
    time_t         atime;     Access time
    time_t         ctime;     Change time
    time_t         btime;     Birth time
    hsize_t        num_attrs; # of attributes attached to object
    H5O_hdr_info_t hdr;       Object header information
    # Extra metadata storage for obj & attributes
    struct {
        H5_ih_info_t obj;  # v1/v2 B-tree & local/fractal heap for groups, B-tree for chunked datasets
        H5_ih_info_t attr; # v2 B-tree & heap for attributes
    } meta_size;
}
*/

herr_t ocpy_callback(hid_t src, const char *name, const H5O_info_t *info, void *dst_) {
    hid_t* dst = static_cast<hid_t*>(dst_);
    int err = 0;
    switch( info->type ){
        case H5O_TYPE_GROUP:
            if(H5Lexists( *dst, name, H5P_DEFAULT) >= 0)
               err = H5Ocopy(src, name, *dst, name, H5P_DEFAULT, H5P_DEFAULT);
        break;
        case H5O_TYPE_DATASET:
            err = H5Ocopy(src, name, *dst, name, H5P_DEFAULT, H5P_DEFAULT);
        break;
        default: /*H5O_TYPE_NAMED_DATATYPE, H5O_TYPE_NTYPES, H5O_TYPE_UNKNOWN */
            ; // nop to keep compiler happy 
    }
    return 0;
}

int main(int argc, char **argv) {
    argparse::ArgumentParser arg("ocpy", "0.0.1");
    arg.add_argument("-i", "--input").required().help("path to input hdf5 file");
    arg.add_argument("-s", "--source").default_value(std::string("/")).help("path to group within hdf5 container");
    arg.add_argument("-o", "--output").required().help("the new hdf5 will be created/or opened rw");
    arg.add_argument("-d", "--destination").default_value(std::string("/")).help("target group");
    
    std::string input, output, source, destination;
    try {
        arg.parse_args(argc, argv);
        input = arg.get<std::string>("--input");
        output = arg.get<std::string>("--output");
        source = arg.get<std::string>("--source");
        destination = arg.get<std::string>("--destination");
        
        // we are using H5CPP in mixed mode, this brings clarity, RAII, error handling mechanism, ... 
        h5::fd_t fd_i = h5::open(input, H5F_ACC_RDONLY);  // be least intrusive, most relaxed
	    h5::fd_t fd_o = h5::create(output, H5F_ACC_TRUNC);
        h5::gr_t dgr{H5I_UNINIT}, sgr = h5::gr_t{H5Gopen(fd_i, source.data(), H5P_DEFAULT)};
        h5::mute();
        if( destination != "/" ){
            char * gname = destination.data();
            dgr = H5Lexists( fd_o, gname, H5P_DEFAULT) >= 0 ?
                h5::gr_t{H5Gcreate(fd_o, gname, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)} : h5::gr_t{H5Gopen(fd_i, gname, H5P_DEFAULT)};
            H5Ovisit(sgr, H5_INDEX_CRT_ORDER, H5_ITER_NATIVE, ocpy_callback, &dgr );
        } else // target is the root directory, use the `h5::fd_t` descriptor 
            H5Ovisit(sgr, H5_INDEX_CRT_ORDER, H5_ITER_NATIVE, ocpy_callback, &fd_o );
        h5::unmute();
    } catch ( const h5::error::any& e ) {
        std::cerr << e.what() << std::endl;
        std::cout << arg;
    }
	return 0;
}