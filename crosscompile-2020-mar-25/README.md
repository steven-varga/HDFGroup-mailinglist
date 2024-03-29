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

# To reproduce platform dependent variables for `cmake`


### Allen Byrn
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

### hdf5/config/cmake/ConversionTests.c
execute `make conversion-dump` to create the list of macro definitions
```bash
#ifdef H5_LDOUBLE_TO_LONG_SPECIAL_TEST    -> H5_LDOUBLE_TO_LONG_SPECIAL_RUN
#ifdef H5_LONG_TO_LDOUBLE_SPECIAL_TEST    -> H5_LONG_TO_LDOUBLE_SPECIAL_RUN
#ifdef H5_LDOUBLE_TO_LLONG_ACCURATE_TEST  -> H5_LDOUBLE_TO_LLONG_ACCURATE_RUN
#ifdef H5_LLONG_TO_LDOUBLE_CORRECT_TEST   -> H5_LLONG_TO_LDOUBLE_CORRECT_RUN
#ifdef H5_NO_ALIGNMENT_RESTRICTIONS_TEST  -> H5_NO_ALIGNMENT_RESTRICTIONS_RUN
#ifdef FC_DUMMY_MAIN
#ifdef H5_DISABLE_SOME_LDOUBLE_CONV_TEST  -> H5_DISABLE_SOME_LDOUBLE_CONV_RUN
```

###  HDFTests.c
execute `make try-dump` to create the list of macro definitions or execute:
`grep -Rn "#ifdef" HDFTests.c |cut -d' ' -f2 | sort -f | tr '\n' ' ' ` from shell.

```bash
DEV_T_IS_SCALAR FC_DUMMY_MAIN FC_DUMMY_MAIN FC_DUMMY_MAIN GETTIMEOFDAY_GIVES_TZ __GLIBC_PREREQ HAVE_ATTRIBUTE HAVE_C99_DESIGNATED_INITIALIZER HAVE_C99_FUNC HAVE_DEFAULT_SOURCE HAVE_DIRECT HAVE_FUNCTION HAVE_IOEO HAVE_LONG_LONG HAVE_OFF64_T HAVE_SOCKLEN_T HAVE_STAT64_STRUCT HAVE_STAT_ST_BLOCKS HAVE_STRUCT_TEXT_INFO HAVE_STRUCT_TIMEZONE HAVE_STRUCT_VIDEOCONFIG HAVE_SYS_SOCKET_H HAVE_SYS_TIME_H HAVE_SYS_TIME_H HAVE_SYS_TIME_H HAVE_SYS_TIME_H HAVE_SYS_TIME_H HAVE_SYS_TYPES_H HAVE_SYS_TYPES_H HAVE_TIMEZONE HAVE___TM_GMTOFF HAVE_TM_GMTOFF HAVE_UNISTD_H PRINTF_LL_WIDTH STDC_HEADERS SYSTEM_SCOPE_THREADS TEST_DIRECT_VFD_WORKS TEST_LFS_WORKS TIME_WITH_SYS_TIME VSNPRINTF_WORKS
```

### Makefile to run all test cases:
```make
#  _____________________________________________________________________________
#  Copyright (c) <2020> <copyright Steven Varga, Toronto, On>
#  _____________________________________________________________________________

all: try-prefix conversion-prefix

# clean target will remove it, so create on demand
setup-dir:
	@mkdir -p build
# 
remove_prefix = $(shell echo '$1' | cut -d'-' -f2)

conversion-src = ConversionTests.c
conversion-list = H5_LDOUBLE_TO_LONG_SPECIAL_TEST H5_LONG_TO_LDOUBLE_SPECIAL_TEST H5_LDOUBLE_TO_LLONG_ACCURATE_TEST 	H5_LLONG_TO_LDOUBLE_CORRECT_TEST H5_NO_ALIGNMENT_RESTRICTIONS_TEST H5_DISABLE_SOME_LDOUBLE_CONV_TEST
conversion-prefix: $(foreach var,$(conversion-list), conversion-$(var))
conversion-%: $(conversion-src) setup-dir
	$(eval value=$(call remove_prefix,$@))
	@$(CC) -o build/$(value) -D$(value) $(conversion-src)
	@./build/$(value) ; echo $(value) $$?

try-src = HDFTests.c
try-list = DEV_T_IS_SCALAR FC_DUMMY_MAIN FC_DUMMY_MAIN FC_DUMMY_MAIN GETTIMEOFDAY_GIVES_TZ __GLIBC_PREREQ HAVE_ATTRIBUTE HAVE_C99_DESIGNATED_INITIALIZER HAVE_C99_FUNC HAVE_DEFAULT_SOURCE HAVE_DIRECT HAVE_FUNCTION HAVE_IOEO HAVE_LONG_LONG HAVE_OFF64_T HAVE_SOCKLEN_T HAVE_STAT64_STRUCT HAVE_STAT_ST_BLOCKS HAVE_STRUCT_TEXT_INFO HAVE_STRUCT_TIMEZONE HAVE_STRUCT_VIDEOCONFIG HAVE_SYS_SOCKET_H HAVE_SYS_TIME_H HAVE_SYS_TIME_H HAVE_SYS_TIME_H HAVE_SYS_TIME_H HAVE_SYS_TIME_H HAVE_SYS_TYPES_H HAVE_SYS_TYPES_H HAVE_TIMEZONE HAVE___TM_GMTOFF HAVE_TM_GMTOFF HAVE_UNISTD_H PRINTF_LL_WIDTH STDC_HEADERS SYSTEM_SCOPE_THREADS TEST_DIRECT_VFD_WORKS TEST_LFS_WORKS TIME_WITH_SYS_TIME VSNPRINTF_WORKS
try-prefix: $(foreach var,$(try-list), try-$(var))
try-%: $(try-src) setup-dir
	$(eval value=$(call remove_prefix,$@))
	@$(CC) -o build/$(value) -D$(value) $(try-src) > /dev/null 2>&1; echo $(value) $$?

clean: 
	@rm -rf build 

.PHONY: all try-prefix conversion-prefix 
```


## Generated output 
The following list probably should contain the platform hash with the list of values. 

```bash
DEV_T_IS_SCALAR 1
FC_DUMMY_MAIN 1
GETTIMEOFDAY_GIVES_TZ 1
__GLIBC_PREREQ 1
HAVE_ATTRIBUTE 0
HAVE_C99_DESIGNATED_INITIALIZER 0
HAVE_C99_FUNC 0
HAVE_DEFAULT_SOURCE 0
HAVE_DIRECT 0
HAVE_FUNCTION 0
HAVE_IOEO 1
HAVE_LONG_LONG 1
HAVE_OFF64_T 1
HAVE_SOCKLEN_T 1
HAVE_STAT64_STRUCT 1
HAVE_STAT_ST_BLOCKS 0
HAVE_STRUCT_TEXT_INFO 1
HAVE_STRUCT_TIMEZONE 1
HAVE_STRUCT_VIDEOCONFIG 1
HAVE_SYS_SOCKET_H 1
HAVE_SYS_TIME_H 1
HAVE_SYS_TYPES_H 1
HAVE_TIMEZONE 0
HAVE___TM_GMTOFF 1
HAVE_TM_GMTOFF 0
HAVE_UNISTD_H 1
PRINTF_LL_WIDTH 1
STDC_HEADERS 0
SYSTEM_SCOPE_THREADS 0
TEST_DIRECT_VFD_WORKS 1
TEST_LFS_WORKS 0
TIME_WITH_SYS_TIME 0
VSNPRINTF_WORKS 0
H5_LDOUBLE_TO_LONG_SPECIAL_TEST 1
H5_LONG_TO_LDOUBLE_SPECIAL_TEST 1
H5_LDOUBLE_TO_LLONG_ACCURATE_TEST 0
H5_LLONG_TO_LDOUBLE_CORRECT_TEST 0
H5_NO_ALIGNMENT_RESTRICTIONS_TEST 0
H5_DISABLE_SOME_LDOUBLE_CONV_TEST 1
```

## yggdrasil: Mose Giordano
```bash
using BinaryBuilder

# Collection of sources required to build HDF5
name = "HDF5"
version = v"1.12.0"

sources = [
    GitSource("https://github.com/steven-varga/hdf5.git",
              "b49b22d6882d97b1ec01d482822955bd8e923203"),
]

# Bash recipe for building across all platforms
script = raw"""
cd ${WORKSPACE}/srcdir/hdf5/
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=${prefix} \
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TARGET_TOOLCHAIN} \
    -DHDF5_BUILD_CPP_LIB=OFF \
    -DONLY_SHARED_LIBS=ON \
    -DHDF5_BUILD_HL_LIB=ON \
    -DHDF5_ENABLE_Z_LIB_SUPPORT=ON \
    -DHDF5_ENABLE_SZIP_SUPPORT=OFF \
    -DHDF5_ENABLE_SZIP_ENCODING=OFF \
    -DBUILD_TESTING=OFF
make -j${nproc}
make install
install_license ${WORKSPACE}/srcdir/hdf5/COPYING*
"""

# These are the platforms we will build for by default, unless further
# platforms are passed in on the command line
platforms = supported_platforms()

# The products that we will ensure are always built
products = [
    LibraryProduct("libhdf5", :libhdf5),
    LibraryProduct("libhdf5_hl", :libhdf5_hl),
]

# Dependencies that must be installed before this package can be built
dependencies = [
    Dependency("Zlib_jll"),
]

# Build the tarballs, and possibly a `build.jl` as well.
build_tarballs(ARGS, name, version, sources, script, platforms, products, dependencies)
```

[link to verify ](https://dev.azure.com/JuliaPackaging/Yggdrasil/_build/results?buildId=1875&view=logs&jobId=96c13d9d-dfd2-51e5-2f76-4a0b3ce09abc&j=12ccd875-e8f0-581b-fff0-8691ac4a723e&t=5d41454d-3d34-5660-28ce-3cc8d5123f5e)
