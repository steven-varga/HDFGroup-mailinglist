/* copyright steven varga, vargalabs 2026, apr 13, Toronto, ON, Canada;  MIT license */

#pragma once

#include <cstdint>
#include <array>          // std::array<T,N> fixed-size container
#include <h5cpp/core>     // H5CPP core: reflection + type registration layer

// --------------------------------------------------------------------------------------
// These are semantic helper types representing 3D entities.
// Also: naming conveys meaning (point vs index).
// point_t  -> continuous space (ℝ³), e.g. geometry, vectors, coordinates
// index_t  -> discrete space   (ℤ³), e.g. grid indices, offsets, extents
// --------------------------------------------------------------------------------------
using point_t = std::array<double,3>;      // (x,y,z) in ℝ³
using index_t = std::array<uint64_t,3>;    // (i,j,k) in ℤ³

namespace h5 {
    // ----------------------------------------------------------------------------------
    // H5CPP customization point:
    // register_struct<T>() tells H5CPP how to map a C++ type T into an HDF5 type (hid_t).
    // Here we explicitly define how std::array<T,3> should be stored:
    // as an HDF5 *array type* of length 3.
    // ----------------------------------------------------------------------------------

    template<> inline hid_t register_struct<point_t>() {
        // HDF5 expects dimensions as an array of hsize_t
        hsize_t dims[1] = {3};  // 1D array of length 3

        // Create HDF5 array type:
        // H5T_IEEE_F64LE  -> 64-bit IEEE floating point, little-endian
        // 1               -> rank (1-dimensional array)
        // dims            -> size = 3
        return H5Tarray_create2(H5T_IEEE_F64LE, 1, dims);
    }

    template<> inline hid_t register_struct<index_t>() {
        hsize_t dims[1] = {3};
        // H5T_NATIVE_ULLONG -> platform-native unsigned long long (uint64_t)
        // This maps directly to uint64_t[3] in HDF5.
        return H5Tarray_create2(H5T_NATIVE_ULLONG, 1, dims);
    }
}

// --------------------------------------------------------------------------------------
// H5CPP macro:
//
// Registers the type with the reflection system so it can be used in:
//  - h5::write
//  - h5::read
//  - dataset creation
//
// Without this, H5CPP would not know how to serialize these types.
// Internally this wires register_struct<T>() into the type system.
// --------------------------------------------------------------------------------------
H5CPP_REGISTER_STRUCT(point_t);
H5CPP_REGISTER_STRUCT(index_t);