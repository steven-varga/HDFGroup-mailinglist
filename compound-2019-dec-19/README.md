# Solution

1. create C++ POD struct in some namespace (or without) to describe dataset "Module"
2. read dataset chunk by chunk, one shot or ...
3. invoke `h5cpp` compiler to fill in missing type descriptor 
4. invoke C++17 compiler to compile and link project against `-lhdf5  -lz -ldl -lm`


## Create POD struct 
manually describe dataset from `h5dump -pH test.h5` as a C++ POD struct
```cpp
#ifndef  H5TEST_STRUCT_01 
#define  H5TEST_STRUCT_01

namespace sn {
	struct record_t {     // POD struct with nested namespace
		double temp;
		double density;
		double B[3];
		double V[3];
		double dm[20];
		double jkq[9];
	};
}
#endif
```

## write software
as if you had all the HDF5 type descriptors available. In fact forget about all the details you won't need them. Just write your software if you didn't know much of HDF5 CAPI:
```cpp
#include <iostream>
#include <vector>
#include "struct.h"
#include <h5cpp/core>
	// generated file must be sandwiched between core and io 
	// to satisfy template dependencies in <h5cpp/io>  
	#include "generated.h"
#include <h5cpp/io>


int main(){
	h5::fd_t fd = h5::create("test.h5", H5F_ACC_TRUNC);
	{ // this is to create the dataset
		h5::create<sn::record_t>(fd, "/Module/g_data", h5::max_dims{70,3,3} );
		// chunk must be set for partial access (huge files):  h5::chunk{1,3,3}
	}

	{ // read entire dataset back
		using T = std::vector<sn::record_t>;
		// for partial read be certain dataset is chunked, see documentation @ sandbox.h5cpp.org
		auto dataset = h5::read<T>(fd,"/Module/g_data");

		for( auto rec:dataset ) // this is your HPC loop
			std::cerr << rec.temp <<" ";
		std::cerr << std::endl;
	}
}
```

## invoke `h5cpp compiler`
The LLVM based source code transformation tool will fill in the details for you: a minimal type descriptor for H5CPP template library to 
save data into HDF5 format.
```cpp
#ifndef H5CPP_GUARD_jAkGV
#define H5CPP_GUARD_jAkGV

namespace h5{
    //template specialization of sn::record_t to create HDF5 COMPOUND type
    template<> hid_t inline register_struct<sn::record_t>(){
        hsize_t at_00_[] ={3};            hid_t at_00 = H5Tarray_create(H5T_NATIVE_DOUBLE,1,at_00_);
        hsize_t at_01_[] ={20};            hid_t at_01 = H5Tarray_create(H5T_NATIVE_DOUBLE,1,at_01_);
        hsize_t at_02_[] ={9};            hid_t at_02 = H5Tarray_create(H5T_NATIVE_DOUBLE,1,at_02_);

        hid_t ct_00 = H5Tcreate(H5T_COMPOUND, sizeof (sn::record_t));
        H5Tinsert(ct_00, "temp",	HOFFSET(sn::record_t,temp),H5T_NATIVE_DOUBLE);
        H5Tinsert(ct_00, "density",	HOFFSET(sn::record_t,density),H5T_NATIVE_DOUBLE);
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
```


# Actual Output:
```
h5cpp  struct.cpp -- -std=c++17 -I/usr/include -I/usr/include/h5cpp-llvm -Dgenerated.h
H5CPP: Copyright (c) 2018     , VargaConsulting, Toronto,ON Canada
LLVM : Copyright (c) 2003-2010, University of Illinois at Urbana-Champaign.
g++ -I/usr/include -o struct.o  -std=c++17 -c struct.cpp
g++ struct.o -lhdf5  -lz -ldl -lm -o struct	
./struct
h5dump -pH test.h5
HDF5 "test.h5" {
GROUP "/" {
   GROUP "Module" {
      DATASET "g_data" {
         DATATYPE  H5T_COMPOUND {
            H5T_IEEE_F64LE "temp";
            H5T_IEEE_F64LE "density";
            H5T_ARRAY { [3] H5T_IEEE_F64LE } "B";
            H5T_ARRAY { [3] H5T_IEEE_F64LE } "V";
            H5T_ARRAY { [20] H5T_IEEE_F64LE } "dm";
            H5T_ARRAY { [9] H5T_IEEE_F64LE } "jkq";
         }
         DATASPACE  SIMPLE { ( 70, 3, 3 ) / ( 70, 3, 3 ) }
         STORAGE_LAYOUT {
            CONTIGUOUS
            SIZE 0
            OFFSET 18446744073709551615
         }
         FILTERS {
            NONE
         }
         FILLVALUE {
            FILL_TIME H5D_FILL_TIME_IFSET
            VALUE  H5D_FILL_VALUE_DEFAULT
         }
         ALLOCATION_TIME {
            H5D_ALLOC_TIME_LATE
         }
      }
   }
}
}
```

# The problem

Hi, one of our applications uses HDF5 files with a compound type. Something like the following (simplified here):

```
HDF5 “test.h5” {
   GROUP “/” {
      ATTRIBUTE “GRID_DIMENSIONS” {
      DATATYPE H5T_STD_I32LE
      DATASPACE SIMPLE { ( 3 ) / ( 3 ) }
   }
   ATTRIBUTE “X_AXIS” {
      DATATYPE H5T_IEEE_F64LE
      DATASPACE SIMPLE { ( 3 ) / ( 3 ) }
   }
   ATTRIBUTE “Z_AXIS” {
      DATATYPE H5T_IEEE_F64LE
      DATASPACE SIMPLE { ( 70 ) / ( 70 ) }
   }
   GROUP “Module” {
      DATASET “g_data” {
      DATATYPE H5T_COMPOUND {
        H5T_IEEE_F64LE “temp”;
        H5T_IEEE_F64LE “density”;
        H5T_ARRAY { [3] H5T_IEEE_F64LE } “B”;
        H5T_ARRAY { [3] H5T_IEEE_F64LE } “V”;
        H5T_ARRAY { [20] H5T_IEEE_F64LE } “dm”;
        H5T_ARRAY { [9] H5T_IEEE_F64LE } “jkq”;
     }
  DATASPACE SIMPLE { ( 70, 3, 3 ) / ( 70, 3, 3 ) }
  }
```

An now I’m trying to find a way in which I can create a virtual dataset or similar so that I can “extract/filter” data from the compound type (the idea was that perhaps I could create, for example, a virtual dataset “density”, taking the actual data from the “density” component of the g_data actual dataset.

Is that something that can be done?

Any ideas/pointers that can help me with this?

Many thanks,


