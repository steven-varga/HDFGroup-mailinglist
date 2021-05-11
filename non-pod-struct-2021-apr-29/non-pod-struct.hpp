/*
 * Copyright (c) 2021 Steven Varga, Toronto,ON Canada
 * Author: Varga, Steven <steven@vargaconsulting.ca>
 */

#ifndef  H5TEST_NON_POD_STRUCT 
#define  H5TEST_NON_POD_STRUCT

#include <hdf5.h>
#include <h5cpp/core>
#include <stddef.h>
#include <string>
#include <string.h>

// your actual backing pod type, at this stage you have to manually create it
// in some later version h5cpp compiler will do this for you
namespace shim {
	struct pod_t {
        using max_lenght = std::integral_constant<size_t,20>;
        size_t id;
        char name[max_lenght::value];
    };
}

namespace intrusive {
    struct non_pod_t { // wrapper to aid C++ template mechanism, zero runtime cost
		using pod_t = ::shim::pod_t;

        size_t id;
        std::string name;
        operator pod_t() const {
            pod_t pod;
            pod.id = this->id;
            strncpy(pod.name, this->name.data(), pod_t::max_lenght::value);
            return pod;
        } 
	};
}

namespace non::intrusive {
    struct non_pod_t { // wrapper to aid C++ template mechanism, zero runtime cost
        size_t id;
        std::string name;
	};
}

namespace h5 {
    //template specialization of shim::pod_t to create HDF5 COMPOUND type
    template<> hid_t inline register_struct<shim::pod_t>(){
        hid_t at_00 = H5Tcopy(H5T_C_S1);
        //H5Tset_size(at_00, ::shim::pod_t::max_lenght::value);
        H5Tset_size(at_00, H5T_VARIABLE);

        hid_t ct_00 = H5Tcreate(H5T_COMPOUND, sizeof (shim::pod_t));
        H5Tinsert(ct_00, "id",	HOFFSET(shim::pod_t,id), H5T_NATIVE_ULONG);
        H5Tinsert(ct_00, "name", HOFFSET(shim::pod_t,name), at_00);

        H5Tclose(at_00); 
        return ct_00;
    };
}
H5CPP_REGISTER_STRUCT(shim::pod_t);

namespace h5 {
    //template specialization of shim::pod_t to create HDF5 COMPOUND type
    template<> hid_t inline register_struct<non::intrusive::non_pod_t>(){
        hid_t at_00 = H5Tcopy(H5T_C_S1);
        H5Tset_size(at_00, H5T_VARIABLE);

        hid_t ct_00 = H5Tcreate(H5T_COMPOUND, sizeof (non::intrusive::non_pod_t));
        H5Tinsert(ct_00, "id",	HOFFSET(non::intrusive::non_pod_t, id), H5T_NATIVE_ULONG);
        H5Tinsert(ct_00, "name", HOFFSET(non::intrusive::non_pod_t, name), at_00);

        H5Tclose(at_00); 
        return ct_00;
    };
}
H5CPP_REGISTER_STRUCT(::non::intrusive::non_pod_t);

#endif