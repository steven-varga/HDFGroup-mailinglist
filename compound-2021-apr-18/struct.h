
/* Copyright (c) 2021 vargaconsulting, Toronto,ON Canada */
#ifndef  H5TEST_STRUCT_01 
#define  H5TEST_STRUCT_01

namespace sn {
	struct record_t {     // POD struct with nested namespace
		double A;
		double B;
		double array_00[5];
		double array_01[5];
	};
}
#endif
