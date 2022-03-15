/* Copyright (c) 2022 vargaconsulting, Toronto,ON Canada */
#ifndef H5CPP_GUARD_NKohX
#define H5CPP_GUARD_NKohX

namespace h5{
    //template specialization of sn::record_t to create HDF5 COMPOUND type
    template<> hid_t inline register_struct<sn::record_t>(){
        hsize_t at_00_[] ={3};            hid_t at_00 = H5Tarray_create(H5T_NATIVE_LDOUBLE,1,at_00_);
        hsize_t at_01_[] ={20};            hid_t at_01 = H5Tarray_create(H5T_NATIVE_LDOUBLE,1,at_01_);
        hsize_t at_02_[] ={9};            hid_t at_02 = H5Tarray_create(H5T_NATIVE_LDOUBLE,1,at_02_);

        hid_t ct_00 = H5Tcreate(H5T_COMPOUND, sizeof (sn::record_t));
        H5Tinsert(ct_00, "temp",	HOFFSET(sn::record_t,temp),H5T_NATIVE_LDOUBLE);
        H5Tinsert(ct_00, "density",	HOFFSET(sn::record_t,density),H5T_NATIVE_LDOUBLE);
        H5Tinsert(ct_00, "B",	HOFFSET(sn::record_t,B),at_00);
        H5Tinsert(ct_00, "V",	HOFFSET(sn::record_t,V),at_00);
        H5Tinsert(ct_00, "dm",	HOFFSET(sn::record_t,dm),at_01);
        H5Tinsert(ct_00, "jkq",	HOFFSET(sn::record_t,jkq),at_02);

        //closing all hid_t allocations to prevent resource leakage
        H5Tclose(at_00); H5Tclose(at_01); H5Tclose(at_02); 

        //if not used with h5cpp framework, but as a standalone code generator then
        //the returned 'hid_t ct_00' must be closed: H5Tclose(ct_00);
        return ct_00;
    };
}
H5CPP_REGISTER_STRUCT(sn::record_t);

#endif
