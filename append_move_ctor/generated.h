/* Copyright (c) 2018 vargaconsulting, Toronto,ON Canada
 *     Author: Varga, Steven <steven@vargaconsulting.ca>
 */
#ifndef H5CPP_GUARD_wRUwW
#define H5CPP_GUARD_wRUwW

namespace h5{
    //template specialization of sn::log to create HDF5 COMPOUND type
    template<> hid_t inline register_struct<sn::log>(){
        hsize_t at_00_[] ={10};            hid_t at_00 = H5Tarray_create(H5T_NATIVE_FLOAT,1,at_00_);

        hid_t ct_00 = H5Tcreate(H5T_COMPOUND, sizeof (sn::log));
        H5Tinsert(ct_00, "idx",	HOFFSET(sn::log,idx),H5T_NATIVE_INT);
        H5Tinsert(ct_00, "payload",	HOFFSET(sn::log,payload),at_00);

        //closing all hid_t allocations to prevent resource leakage
        H5Tclose(at_00); 

        //if not used with h5cpp framework, but as a standalone code generator then
        //the returned 'hid_t ct_00' must be closed: H5Tclose(ct_00);
        return ct_00;
    };
}
H5CPP_REGISTER_STRUCT(sn::log);

#endif
