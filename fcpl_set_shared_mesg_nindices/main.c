
#include <hdf5.h>
#include <stdio.h>

int main() {
/*
H5Pset_shared_mesg_nindexes sets the number of shared object header message indexes in the specified file creation property list.
This setting determines the number of shared object header message indexes that will be available in files created with this property list. These indexes can then be configured with H5Pset_shared_mesg_index.
If nindexes is set to 0 (zero), shared object header messages are disabled in files created with this property list.
*/
	char name[1024];
	hid_t fcpl_id, fd_id;
	herr_t err;
	unsigned N=9, nidx;

	fcpl_id =  H5Pcreate(H5P_FILE_CREATE);
	err = H5Pset_shared_mesg_nindexes( fcpl_id, N );
	fd_id = H5Fcreate("test.h5", H5F_ACC_TRUNC, fcpl_id, H5P_DEFAULT );
	H5Pget_shared_mesg_nindexes( fcpl_id, &nidx );
	printf("results: %i - %i = %i \n", N, nidx, H5O_SHMESG_MAX_NINDEXES);

	H5Pclose(fcpl_id);
	H5Fclose(fd_id);
}

