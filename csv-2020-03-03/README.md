# CSV to HDF5 

Public domain CSV example file obtained from [this link](https://catalog.data.gov/dataset?res_format=CSV) The CSV library is [Fast C++ CSV Parser](https://github.com/ben-strasser/fast-cpp-csv-parser)

# C++/C representation

arbitrary pod struct can be represented in HDF5 format, one easy representation of strings is character array. An alternative --often better performing --representation would be to factor out strings from numerical data, then save them in separate datasets.
```
#ifndef  CSV2H5_H 
#define  CSV2H5_H

/*define C++ representation as POD struct*/
struct input_t {
	long MasterRecordNumber;
	unsigned int Hour;
	double Latitude;
	double Longitude;
	char ReportedLocation[20]; // character arrays are supported
};
#endif
```

Reading the CSV is rather easy thanks to [Fast C++ CSV Parser](https://github.com/ben-strasser/fast-cpp-csv-parser), a single header file `csv.h` is attached to the project. Not only fast and simple but also elegantly allows to specify specific columns marked as ncols: `N_COLS`

```
io::CSVReader<N_COLS> in("input.csv"); // number of cols may be less, than total columns in a row, we're to read only 5
in.read_header(io::ignore_extra_column, "Master Record Number", "Hour", "Reported_Location","Latitude","Longitude");
[...]
while(in.read_row(row.MasterRecordNumber, row.Hour, ptr, row.Latitude, row.Longitude)){
	[...]
```

The HDF5 part is matching in simplicity:
```
	h5::fd_t fd = h5::create("output.h5",H5F_ACC_TRUNC);
	h5::pt_t pt = h5::create<input_t>(fd,  "monroe-county-crash-data2003-to-2015.csv",
				 h5::max_dims{H5S_UNLIMITED}, h5::chunk{1024} | h5::gzip{9} ); // compression, chunked, unlimited size
	[...]
	while(...){
		h5::append(pt, row); // append operator uses internal buffers to cache and convert row insertions to block/chunk operations
	}
	[...]
```

The TU translation unit is scanned with LLVM based `h5cpp` compiler and the necessary hdf5 specific type descriptors are produced:
```
#ifndef H5CPP_GUARD_mzMuQ
#define H5CPP_GUARD_mzMuQ

namespace h5{
    //template specialization of input_t to create HDF5 COMPOUND type
    template<> hid_t inline register_struct<input_t>(){
        //hsize_t at_00_[] ={20};            hid_t at_00 = H5Tarray_create(H5T_STRING,20,at_00_);
		hid_t at_00 = H5Tcopy (H5T_C_S1); H5Tset_size(at_00, 20);
        hid_t ct_00 = H5Tcreate(H5T_COMPOUND, sizeof (input_t));
        H5Tinsert(ct_00, "MasterRecordNumber",	HOFFSET(input_t,MasterRecordNumber),H5T_NATIVE_LONG);
        H5Tinsert(ct_00, "Hour",	HOFFSET(input_t,Hour),H5T_NATIVE_UINT);
        H5Tinsert(ct_00, "Latitude",	HOFFSET(input_t,Latitude),H5T_NATIVE_DOUBLE);
        H5Tinsert(ct_00, "Longitude",	HOFFSET(input_t,Longitude),H5T_NATIVE_DOUBLE);
        H5Tinsert(ct_00, "ReportedLocation",	HOFFSET(input_t,ReportedLocation),at_00);

        //closing all hid_t allocations to prevent resource leakage
        H5Tclose(at_00); 

        //if not used with h5cpp framework, but as a standalone code generator then
        //the returned 'hid_t ct_00' must be closed: H5Tclose(ct_00);
        return ct_00;
    };
}
H5CPP_REGISTER_STRUCT(input_t);

#endif
```

The entire project can be [downloaded from this link](https://github.com/steven-varga/HDFGroup-mailinglist/tree/master/csv-2020-03-03) but for completeness here is the source file:
```
#include "csv.h"
#include "struct.h"

#include <h5cpp/core>
	#include "generated.h"
#include <h5cpp/io>

int main(){

	// create HDF5 container
	h5::fd_t fd = h5::create("output.h5",H5F_ACC_TRUNC);
	// create dataset `h5::ds_t` handle is seamlessly cast to `h5::pt_t` packet table handle  
	h5::pt_t pt = h5::create<input_t>(fd,  "monroe-county-crash-data2003-to-2015.csv",
				 h5::max_dims{H5S_UNLIMITED}, h5::chunk{1024} | h5::gzip{9} ); // compression, chunked, unlimited size

	constexpr unsigned N_COLS = 5;
	io::CSVReader<N_COLS> in("input.csv"); // number of cols may be less, than total columns in a row, we're to read only 5
	in.read_header(io::ignore_extra_column, "Master Record Number", "Hour", "Reported_Location","Latitude","Longitude");
	input_t row;                           // buffer to read line by line
	char* ptr;      // indirection, as `read_row` doesn't take array directly
	while(in.read_row(row.MasterRecordNumber, row.Hour, ptr, row.Latitude, row.Longitude)){
		strncpy(row.ReportedLocation, ptr, 20);
		h5::append(pt, row);
		std::cout << std::string(ptr) << "\n";
	}
	// RAII closes all handles
}
```



