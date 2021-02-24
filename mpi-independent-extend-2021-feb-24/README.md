# **Independent datasets for MPI processes**
A previous post mentioned that it may become possible to have indpendant datasets for independant processes, reducing the need for collective calls such as when extending a dataset. That post is now 10yrs old. Has anything changed on this front? The link in the previous post is no longer working.

**Full story:**
I have two seperate processes generating data, one generates values rapidly from an oscilicope, the other generates larger data more slowly from a camera.Therefore they are looping at different speeds. I would like to write the data from each process into its own data set, this includes extending the data set, which is a collective call. Since the two threads do not interact with the other’s dataset can this be made indepenantly? Mainly this is desired to save time by preventing processes waiting for each other.

## MWE: `mpi-extend`
creates a container, and `rank` many datasets with random sizes using collective calls. Once the rig is set up, by default it extends a single dataset with collectiv ecall to demonstrate fitness of the rig but not the actual problem. By adjusting/uncommenting lines marked with `NOTE:` one can trigger the code relevant to the OP's question.  


## identification:
The MWE indeed shows for `phdf5v1.10.6` all processes must participate in `H5Dset_extent` call, otherwise the program will hand indefinetally. 

### output:
```
mpic++ -o mpi-extend.o   -std=c++17 -O3  -I/usr/local/include -Wno-narrowing -c mpi-extend.cpp
mpic++ mpi-extend.o -lz -ldl -lm  -lhdf5 -o mpi-extend	
srun -n 4 -w io ./mpi-extend
[rank]	2	[total elements]	0
[dimensions]	current: {346,0}	maximum: {346,inf}
[selection]	start: {0,0}	end:{345,inf}
[rank]	2	[total elements]	0
[dimensions]	current: {346,0}	maximum: {346,inf}
[selection]	start: {0,0}	end:{345,inf}
[rank]	2	[total elements]	0
[dimensions]	current: {346,0}	maximum: {346,inf}
[selection]	start: {0,0}	end:{345,inf}
[rank]	2	[total elements]	0
[dimensions]	current: {346,0}	maximum: {346,inf}
[selection]	start: {0,0}	end:{345,inf}
{346,0}{346,0}{346,0}{346,0}h5ls -r mpi-extend.h5
/                        Group
/io-00                   Dataset {346, 400/Inf}
/io-01                   Dataset {465, 0/Inf}
/io-02                   Dataset {136, 0/Inf}
/io-03                   Dataset {661, 0/Inf}

```


## workarounds:

- ZeroMQ based solution with a single writer thread


# requirements:

- [h5cpp v1.10.6-1](http://h5cpp.org)
- PHDF5 C base library, no high level API or beuilt in C++ API is needed works with:  `parallel`
- c++17 or higher compiler
