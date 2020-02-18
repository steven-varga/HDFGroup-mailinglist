
#include <hdf5.h>
#include <stdio.h>

int main() {
/*
H5Pset_istore_k sets the size of the parameter used to control the B-trees for indexing chunked datasets. This function is valid only for file creation property lists.
ik is one half the rank of a tree that stores chunked raw data. On average, such a tree will be 75% full, or have an average rank of 1.5 times the value of ik.
The HDF5 library uses (ik*2) as the maximum # of entries before splitting a B-tree node. Since only 2 bytes are used in storing # of entries for a B-tree node in an HDF5 file, (ik*2) cannot exceed 65536. The default value for ik is 32.

*/
	char name[1024];
	hid_t fcpl_id, fd_id;
	herr_t err;
	unsigned ik;
	//unsigned max = 16384+1;
	unsigned max = 65536;
	for(unsigned i=32; i < max; i*=2) {
		sprintf(name, "name_%i.h5", i);
		fcpl_id =  H5Pcreate(H5P_FILE_CREATE);
		err = H5Pset_istore_k(fcpl_id, i);
		fd_id = H5Fcreate( name, H5F_ACC_TRUNC, fcpl_id, H5P_DEFAULT );
		H5Pget_istore_k(fcpl_id, &ik );
		printf("results: %i - %i = %i ", i, ik, i - ik);

		H5Pclose(fcpl_id);
		H5Fclose(fd_id);
	}
}

