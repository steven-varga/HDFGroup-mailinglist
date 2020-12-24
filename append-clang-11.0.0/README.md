# H5CPP with LLVM 11.0.0

For reproducibility I am using [spack]() package manager:
```
spack install llvm@11.0.0+shared_libs
spack install hdf5@1.10.6%gcc@10.2.0~cxx~debug~fortran~hl~java~mpi+pic+shared+szip~threadsafe api=v110
```

Looking at the `spack` version of `h5cpp` -- which is set up for MPI, hence will not do well here -- lets use the standard `h5cpp@1.10.4-6` header libraries installed in `/usr/local/include`. This is the latest version, which supports upto `v110` HDF5 API calls. If you are interested in `v112` or `v114` you have to wait me to release the new codebase.

Anyhow, let's load the environment:
```
spack load hdf5@1.10.6
spack load llvm@11.0.0
```

Now we have both `hdf5` and `clang++` available, check it out:
```
clang++ --version
clang version 11.0.0
Target: x86_64-unknown-linux-gnu
Thread model: posix
InstalledDir: /opt/spack/opt/spack/gcc-10.2.0/llvm/11.0.0-7i2mf/bin
```

and here comes hdf5, we are in business!
``
h5dump --version
h5dump: Version 1.10.6`
```

`make` does all the steps, here is the result:
```
h5dump  example.h5 
HDF5 "example.h5" {
GROUP "/" {
   DATASET "stream of integral" {
      DATATYPE  H5T_STD_I32LE
      DATASPACE  SIMPLE { ( 6, 3, 5 ) / ( H5S_UNLIMITED, 3, 5 ) }
      DATA {
      (0,0,0): 1, 2, 3, 4, 5,
      (0,1,0): 6, 7, 8, 9, 10,
      (0,2,0): 11, 12, 13, 14, 15,
      (1,0,0): 16, 17, 18, 19, 20,
      (1,1,0): 21, 22, 23, 24, 25,
      (1,2,0): 26, 27, 28, 29, 30,
      (2,0,0): 31, 32, 33, 34, 35,
      (2,1,0): 36, 37, 38, 39, 40,
      (2,2,0): 41, 42, 43, 44, 45,
      (3,0,0): 46, 47, 48, 49, 50,
      (3,1,0): 51, 52, 53, 54, 55,
      (3,2,0): 56, 57, 58, 59, 60,
      (4,0,0): 61, 62, 63, 64, 65,
      (4,1,0): 66, 67, 68, 69, 70,
      (4,2,0): 71, 72, 73, 74, 75,
      (5,0,0): 76, 77, 78, 79, 80,
      (5,1,0): 81, 82, 83, 3, 3,
      (5,2,0): 3, 3, 3, 3, 3
      }
   }
}
}
```

**all check out** h5cpp works with `llvm 11.0.0`