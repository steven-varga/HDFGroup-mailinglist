/* Copyright (c) 2018 vargaconsulting, Toronto,ON Canada
 *     Author: Varga, Steven <steven@vargaconsulting.ca>
 */
#ifndef H5CPP_GUARD_NKohX
#define H5CPP_GUARD_NKohX

namespace h5{
    //template specialization of sn::record_t to create HDF5 COMPOUND type
    template<> hid_t inline register_struct<sn::record_t>(){
        hsize_t at_00_[] ={5};            hid_t at_00 = H5Tarray_create(H5T_NATIVE_DOUBLE,1,at_00_);
        hsize_t at_01_[] ={5};            hid_t at_01 = H5Tarray_create(H5T_NATIVE_DOUBLE,1,at_01_);

        hid_t ct_00 = H5Tcreate(H5T_COMPOUND, sizeof (sn::record_t));
        H5Tinsert(ct_00, "A",	HOFFSET(sn::record_t,A),H5T_NATIVE_DOUBLE);
        H5Tinsert(ct_00, "B",	HOFFSET(sn::record_t,B),H5T_NATIVE_DOUBLE);
        H5Tinsert(ct_00, "array_00",	HOFFSET(sn::record_t,array_00),at_00);
        H5Tinsert(ct_00, "array_01",	HOFFSET(sn::record_t,array_01),at_01);

        //closing all hid_t allocations to prevent resource leakage
        H5Tclose(at_00); H5Tclose(at_01); 

        //if not used with h5cpp framework, but as a standalone code generator then
        //the returned 'hid_t ct_00' must be closed: H5Tclose(ct_00);
        return ct_00;
    };
}
H5CPP_REGISTER_STRUCT(sn::record_t);

#endif
