
/* Copyright (c) 2019 vargaconsulting, Toronto,ON Canada
 * Author: Varga, Steven <steven@vargaconsulting.ca>
 */

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
