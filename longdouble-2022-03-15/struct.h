
/* Copyright (c) 2022 vargaconsulting, Toronto,ON Canada License: MIT */

#ifndef  H5TEST_LONGDOUBLE
#define  H5TEST_LONGDOUBLE

namespace sn {
	struct record_t {     // POD struct with nested namespace
		long double temp;
		long double density;
		long double B[3];
		long double V[3];
		long double dm[20];
		long double jkq[9];
	};
}
#endif
