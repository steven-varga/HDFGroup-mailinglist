## problem: H5Pset_shared_mesg_nindexes(fcpl_id, N) fails with N > 8  
**HDF5 Version:** 1.10.6
[documentation](https://support.hdfgroup.org/HDF5/doc/RM/RM_H5P.html#Property-SetSharedMesgNIndexes) doesn't mention 
`N <= H5O_SHMESG_MAX_NINDEXES` the `N` argument is limited to maximum `H5O_SHMESG_MAX_NINDEXES`

```
int main() {
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
```

#output of `make test`
```
cc main.c -lhdf5  -lz -ldl -lm -o set_shared_mesg_nindices	
./set_shared_mesg_nindices
HDF5-DIAG: Error detected in HDF5 (1.10.6) thread 0:
  #000: H5Pfcpl.c line 831 in H5Pset_shared_mesg_nindexes(): number of indexes is greater than H5O_SHMESG_MAX_NINDEXES
    major: Invalid arguments to routine
    minor: Out of range
results: 9 - 0 = 8
```
Documenting that `N <= H5O_SHMESG_MAX_NINDEXES` can help to prevent error message

#linking:
```
	linux-vdso.so.1 (0x00007fff929f5000)
	libhdf5.so.103 => /usr/local/lib/libhdf5.so.103 (0x00007ff443b55000)
	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007ff443764000)
	libz.so.1 => /lib/x86_64-linux-gnu/libz.so.1 (0x00007ff443547000)
	libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007ff443343000)
	libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007ff442fa5000)
	/lib64/ld-linux-x86-64.so.2 (0x00007ff444332000)
```

