# **writing references and data to referened regions**


The [link to the problem](https://forum.hdfgroup.org/t/h5i-dec-ref-hangs/8104/21?u=steven)

```
If rank 0 -> Open H5 file in serial mode.
If rank 0 -> create 10 groups
If rank 0 -> inside each group, create a float type dataset (10 in total)
If rank 0 -> inside each group, create a region reference type dataset (10 in total), one dimension, size 10
If rank 0 -> add attributes to the datasets above (vlen strings)
If rank 0 -> Close H5 file in serial mode.
Barrier
Open the file in parallel mode.
Each rank writes to its own float type dataset (rank = index of the dataset).
Close the file in parallel mode.
If rank 0 -> open file in serial mode
If rank 0 -> for each group -> take a region reference dataset and write a 64x64x1 region reference to a float type dataset into it (just an example, I think it doesn't matter which dataset you choose for the region reference)
If rank 0 -> close the file
```

#output:
```
mpic++ -o mpi-reference-test.o   -std=c++17 -DFMT_HEADER_ONLY -c mpi-reference-test.cpp
mpic++ mpi-reference-test.o -lhdf5  -lz -ldl -lm  -o mpi-reference-test
srun -n 8 -w io ./mpi-reference-test
h5ls -r mpi-reference.h5
/                        Group
/000                     Group
/000/dataset             Dataset {1489, 2048, 2}
/000/reference           Dataset {8}
/001                     Group
/001/dataset             Dataset {1489, 2048, 2}
/001/reference           Dataset {8}
/002                     Group
/002/dataset             Dataset {1489, 2048, 2}
/002/reference           Dataset {8}
/003                     Group
/003/dataset             Dataset {1489, 2048, 2}
/003/reference           Dataset {8}
/004                     Group
/004/dataset             Dataset {1489, 2048, 2}
/004/reference           Dataset {8}
/005                     Group
/005/dataset             Dataset {1489, 2048, 2}
/005/reference           Dataset {8}
/006                     Group
/006/dataset             Dataset {1489, 2048, 2}
/006/reference           Dataset {8}
/007                     Group
/007/dataset             Dataset {1489, 2048, 2}
/007/reference           Dataset {8
```



# requirements:

- [h5cpp master](http://h5cpp.org)
- PHDF5 C base library, no high level API or beuilt in C++ API is needed works with:  `parallel`
- c++17 or higher compiler
