/*
 * Copyright (c) 2018 - 2022 vargaconsulting, Toronto,ON Canada
 * Author: Varga, Steven <steven@vargaconsulting.ca>
 */
#ifndef H5CPP_TCONVERSION_HPP
#define H5CPP_TCONVERSION_HPP

#include <hdf5.h>
#include <iostream>
#include "H5Tall.hpp"
#include <vector>

namespace h5::impl::conversion {
    template <class T, int N=1> using array_t = T[N];

    using init_t = herr_t (*)(::hid_t, ::hid_t, void**, size_t nelem);
    using convert_t = herr_t (*)(void*, size_t, size_t);
    using free_t = herr_t (*)(void*);
    using cs_t = ::h5::character_set_t; 

    template<cs_t from_t, cs_t to_t>
    herr_t string(::hid_t src, ::hid_t dst, void** data, size_t nelem){
        std::cout << "init:{"<<H5Tget_cset(src) << " " << H5Tget_cset(dst) << " n: " << nelem<< "}"; 
        std::cout << "["<< H5Tis_variable_str(src) <<" " <<  H5Tis_variable_str(dst) <<"]\n";
        return 0;
    }
    template<>
    herr_t string<cs_t::utf8, cs_t::utf8>(::hid_t src, ::hid_t dst, void** data, size_t nelem){

        std::cout << "utf8: init:{"<<H5Tget_cset(src) << " " << H5Tget_cset(dst) << "}"; 
        std::cout << "["<< H5Tis_variable_str(src) <<" " <<  H5Tis_variable_str(dst) <<"]\n";
        return -1;
    }

  herr_t my_convert(void* data, size_t N, size_t stride ){
        const char** ptr = static_cast<const char**>(data);
        std::cout << *ptr << "\n";
        std::cout << *(ptr+1) << "\n";
        std::cout <<"<convert: " << N << ">\n";
        return 0;
    }
    herr_t no_op(void* data){
        std::cout << "<char free>\n"; 
        return 0;
    }

    template <init_t init, convert_t convert, free_t free>
    ::herr_t delegate(::hid_t src_id, ::hid_t dst_id, H5T_cdata_t *cdata, size_t nelmts, 
        size_t buf_stride, size_t bkg_stride, void *buf, void *bkg, ::hid_t dset_xfer_plist) {
            switch( cdata->command){
                case H5T_CONV_INIT: return init(src_id, dst_id, &cdata->priv, nelmts);    break;
                case H5T_CONV_CONV: return convert(buf, nelmts, buf_stride); break;
                case H5T_CONV_FREE:	return free(buf);
                break; 
            }
        return -1;
    }

    template <init_t init, convert_t convert, free_t free>
    struct converter_t {

        converter_t(const std::string& name) {
            H5Tregister(H5T_PERS_SOFT, name.data(), h5::create<char[3][4]>(), h5::create<std::string>(), delegate<init, convert, free>);
        }
    };
    
}




namespace {
    namespace co = h5::impl::conversion;

    const static co::converter_t str_flen2vlen = co::converter_t<co::string<co::cs_t::utf16 ,co::cs_t::utf8>, co::my_convert, co::no_op>("str_flen2vlen");
}

#endif
/*
herr_t H5Tregister( H5T_pers_t type, const char * name, hid_t src_id, hid_t dst_id, H5T_conv_t func )

*/