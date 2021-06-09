#  Copyright (c) june 08, 2021 Steven Varga, Toronto,ON Canada

## NOTE: we are to use MPIC as opposed to C++ which is depricated 
# mpicc --showme:compile
# mpicc --showme:link
CXX=mpic++
CXXFLAGS =  -std=c++17 -DFMT_HEADER_ONLY
LIBS =  -lhdf5  -lz -ldl -lm 

test: mpi-reference-test
	srun -n 8 -w io ./mpi-reference-test
	h5ls -r mpi-reference.h5

benchmark: hdf5-fixed-length-bench

%.o : %.cpp 
	$(CXX) -o $@  $(CPPFLAGS) $(CXXFLAGS) -c $^

%-test: %-test.o 
	$(CXX) $^ $(LIBS) -o $@


%-bench: %-bench.o 
	$(CXX) $^ $(LIBS) -o $@

clean:
	@$(RM) *.o *.h5 *-test *-bench *.callgrind *.prof  
	
.PHONY: test

dist-clean: clean
.PHONY: test
