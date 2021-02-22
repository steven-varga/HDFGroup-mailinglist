# **hdf5 object copy**
copies a tree of objects from one hdf5 into another. To generate random input please [check out this project](https://github.com/steven-varga/h5rnd)


# usage:
```
Usage: ocpy [options] 

Optional arguments:
-h --help               shows help message and exits
-v --version            prints version information and exits
-i --input              path to input hdf5 file [required]
-s --source             path to group within hdf5 container [default: "/"]
-o --output             the new hdf5 will be created/or opened rw [required]
-d --destination        target group [default: "/"]
```

![example input](original.svg)

# requirements:

- [h5cpp v1.10.6-1](http://h5cpp.org)
- [argparse](https://github.com/p-ranav/argparse)  included with this project
- HDF5 C base library, no high level API or beuilt in C++ API is needed works with:  `serial|parallel`
- c++17 or higher compiler
