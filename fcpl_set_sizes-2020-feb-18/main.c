
#include <hdf5.h>
#include <stdio.h>

int main() {
/*
H5Pset_sizes sets the byte size of the offsets and lengths used to address objects in an HDF5 file. This function is only valid for file creation property lists. Passing in a value of 0 for one of the sizeof_... parameters retains the current value. The default value for both values is the same as sizeof(hsize_t) in the library (normally 8 bytes). Valid values currently are 2, 4, 8 and 16.
*/

size_t offset[] = {2,4,8,16};
size_t length[] = {2,4,8,16};
char name[1024];
hid_t fcpl_id, fd_id;
herr_t err;

size_t N = 4;
	for( size_t i=0; i<N; i++) {
		for( size_t j=0; j<N; j++ ) {
			sprintf(name, "name_%ld_%ld.h5", offset[i], length[j]);
			fcpl_id =  H5Pcreate(H5P_FILE_CREATE);
			err = H5Pset_sizes(fcpl_id, offset[i], length[j]);
			fd_id = H5Fcreate( name, H5F_ACC_TRUNC, fcpl_id,H5P_DEFAULT );
			H5Pclose(fcpl_id);
			H5Fclose(fd_id);
		}
	}
}

