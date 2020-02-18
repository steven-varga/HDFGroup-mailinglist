## problem: H5Pset_sizes(fcpl_id, offset, length) crashes
**HDF5 Version:** 1.10.6

When either of the values take on value `16` library crashes, tested against  HDF5 1.10.6, others maybe affected
```
size_t offset[] = {2,4,8,16};
size_t length[] = {2,4,8,16};

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

```

#output of `make test`
```
cc main.c -lhdf5  -lz -ldl -lm -o set_sizes	
./set_sizes
free(): invalid pointer
Makefile:12: recipe for target 'test' failed
make: *** [test] Aborted (core dumped)
```

Change `size_t N=4` to 	`size_t N=3` tp reduce test range from `{2,4,8,16} X {2,4,8,16}` to  `{2,4,8} X {2,4,8}`
*notice the missing `16` which is valid input according to documentation.*


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

