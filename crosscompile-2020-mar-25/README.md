# [Posted problem](https://forum.hdfgroup.org/t/cross-compiling-for-windows/6735)

Hello, I’m trying to cross-compile HDF5 using BinaryBuilder 3, but it’s still not clear to me the setup to use. I’ve tried calling CMake in the following way:

cmake .. -DCMAKE_INSTALL_PREFIX=${prefix} \
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TARGET_TOOLCHAIN} \
    -DHDF5_BUILD_CPP_LIB=OFF \
    -DONLY_SHARED_LIBS=ON \
    -DHDF5_BUILD_HL_LIB=ON \
    -DHDF5_ENABLE_Z_LIB_SUPPORT=ON \
    -DHDF5_ENABLE_SZIP_SUPPORT=OFF \
    -DHDF5_ENABLE_SZIP_ENCODING=OFF \
    -DBUILD_TESTING=OFF

# Allen Byrn
According to CMake, in the cross-compile toolchain you pre-set those variables to the results of the try run. Yes you need to know what those values are. I would start with the info from the CMake sites and go from there.
```
H5_LDOUBLE_TO_LONG_SPECIAL_RUN (advanced)
H5_LDOUBLE_TO_LONG_SPECIAL_RUN__TRYRUN_OUTPUT (advanced)
H5_LONG_TO_LDOUBLE_SPECIAL_RUN (advanced)
H5_LONG_TO_LDOUBLE_SPECIAL_RUN__TRYRUN_OUTPUT (advanced)
H5_LDOUBLE_TO_LLONG_ACCURATE_RUN (advanced)
H5_LDOUBLE_TO_LLONG_ACCURATE_RUN__TRYRUN_OUTPUT (advanced)
H5_LLONG_TO_LDOUBLE_CORRECT_RUN (advanced)
H5_LLONG_TO_LDOUBLE_CORRECT_RUN__TRYRUN_OUTPUT (advanced)
H5_DISABLE_SOME_LDOUBLE_CONV_RUN (advanced)
H5_DISABLE_SOME_LDOUBLE_CONV_RUN__TRYRUN_OUTPUT (advanced)
H5_NO_ALIGNMENT_RESTRICTIONS_RUN (advanced)
H5_NO_ALIGNMENT_RESTRICTIONS_RUN__TRYRUN_OUTPUT (advanced)
```

# ConversionTest.c

```
#ifdef H5_LDOUBLE_TO_LONG_SPECIAL_TEST    -> H5_LDOUBLE_TO_LONG_SPECIAL_RUN
#ifdef H5_LONG_TO_LDOUBLE_SPECIAL_TEST    -> H5_LONG_TO_LDOUBLE_SPECIAL_RUN
#ifdef H5_LDOUBLE_TO_LLONG_ACCURATE_TEST  -> H5_LDOUBLE_TO_LLONG_ACCURATE_RUN
#ifdef H5_LLONG_TO_LDOUBLE_CORRECT_TEST   -> H5_LLONG_TO_LDOUBLE_CORRECT_RUN
#ifdef H5_NO_ALIGNMENT_RESTRICTIONS_TEST  -> H5_NO_ALIGNMENT_RESTRICTIONS_RUN
#ifdef FC_DUMMY_MAIN
#ifdef H5_DISABLE_SOME_LDOUBLE_CONV_TEST  -> H5_DISABLE_SOME_LDOUBLE_CONV_RUN
```

# Makefile
`make clean && make` iterates through the list, compiles then executes each target, printing out the return code:

```
steven@honshu:~/projects/crosscompile-2020-mar-25$ make clean && make
H5_LDOUBLE_TO_LONG_SPECIAL_TEST 1
H5_LONG_TO_LDOUBLE_SPECIAL_TEST 1
H5_LDOUBLE_TO_LLONG_ACCURATE_TEST 0
H5_LLONG_TO_LDOUBLE_CORRECT_TEST 0
H5_NO_ALIGNMENT_RESTRICTIONS_TEST 0
H5_DISABLE_SOME_LDOUBLE_CONV_TEST 1
```

