## problem: H5Pset_istore_k(fcpl_id, ik) fails  
**HDF5 Version:** 1.10.6

[documentation suggest `ik=65536`](https://support.hdfgroup.org/HDF5/doc/RM/RM_H5P.html#Property-SetIstoreK) as max value, whereas library accepts only `ik < 16384+1`  HDF5 1.10.6, others maybe affected
```
int main() {
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
```

#output of `make test`
```
cc main.c -lhdf5  -lz -ldl -lm -o set_istore_k	
./set_istore_k
results: 32 - 32 = 0 
results: 64 - 64 = 0 
results: 128 - 128 = 0 
results: 256 - 256 = 0 
results: 512 - 512 = 0 
results: 1024 - 1024 = 0 
results: 2048 - 2048 = 0 
results: 4096 - 4096 = 0 
results: 8192 - 8192 = 0 
results: 16384 - 16384 = 0 
HDF5-DIAG: Error detected in HDF5 (1.10.6) thread 0:
  #000: H5Pfcpl.c line 645 in H5Pset_istore_k(): istore IK value exceeds maximum B-tree entries
    major: Invalid arguments to routine
    minor: Bad value
results: 32768 - 32 = 32736
```

adjusting the documentation may be a possible fix

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

