#  Copyright (c) Feb 18, 2021 Steven Varga, Toronto,ON Canada


## NOTE: we are to use MPIC as opposed to C++ which is depricated 
# mpicc --showme:compile
# mpicc --showme:link
CXX=mpic++
CXXFLAGS = -std=c++17 -O3  -I/usr/local/include -Wno-narrowing
LIBS =   -lz -ldl -lm  -lhdf5

test: test-mpi-extend

test-mpi-extend: mpi-extend
	srun -n 4 -w io ./mpi-extend
	h5ls -r mpi-extend.h5

%.o : %.cpp 
	$(CXX) -o $@  $(CPPFLAGS) $(CXXFLAGS) -c $^

mpi-extend: mpi-extend.o
	$(CXX) $^ $(LIBS) -o $@	

clean:
	@$(RM) *.o mpi-extend *.h5 *.callgrind *.prof  

dist-clean: clean
.PHONY: test

