#include <hdf5.h>
#include <h5cpp/all>
#include <stdint.h>

namespace h5::bench {
    struct test_t {
        using max_lenght = std::integral_constant<size_t,60>;
        uint16_t warmup;
        uint16_t iteration;
        uint64_t x;

        double mean_runtime;
        double std_runtime;

        double mean_throughput;
        double std_throughput;

        char name[max_lenght::value];
    };
}

namespace h5 {
    //template specialization of shim::pod_t to create HDF5 COMPOUND type
    template<> hid_t inline register_struct<h5::bench::test_t>(){
        hid_t name_t = H5Tcopy(H5T_C_S1);
        H5Tset_size(name_t, H5T_VARIABLE);

        hid_t ct_00 = H5Tcreate(H5T_COMPOUND, sizeof (h5::bench::test_t));
        H5Tinsert(ct_00, "warmup",	    HOFFSET(h5::bench::test_t,warmup), H5T_NATIVE_USHORT);
        H5Tinsert(ct_00, "iteration",	HOFFSET(h5::bench::test_t,iteration), H5T_NATIVE_USHORT);
        H5Tinsert(ct_00, "x",	        HOFFSET(h5::bench::test_t,x), H5T_NATIVE_ULLONG);
       
        H5Tinsert(ct_00, "mean_runtime",HOFFSET(h5::bench::test_t,mean_runtime), H5T_NATIVE_DOUBLE);
        H5Tinsert(ct_00, "std_runtime",	HOFFSET(h5::bench::test_t,std_runtime), H5T_NATIVE_DOUBLE);

        H5Tinsert(ct_00, "mean_throughput",	HOFFSET(h5::bench::test_t,mean_throughput), H5T_NATIVE_DOUBLE);
        H5Tinsert(ct_00, "std_throughput",	HOFFSET(h5::bench::test_t,std_throughput), H5T_NATIVE_DOUBLE);

        H5Tinsert(ct_00, "name", HOFFSET(h5::bench::test_t,name), name_t);

        H5Tclose(name_t); 
        return ct_00;
    };
}
H5CPP_REGISTER_STRUCT(h5::bench::test_t);
