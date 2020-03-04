/* Copyright (c) 2018 vargaconsulting, Toronto,ON Canada
 * Author: Varga, Steven <steven@vargaconsulting.ca>
 */

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
