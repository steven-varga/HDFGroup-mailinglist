
CXXFLAGS =  -std=c++17 -Wno-attributes
LIBS =  -lhdf5  -lz -ldl -lm
INCLUDES = -I/usr/local/include -I/usr/include -I./include

programs=is-pod-test non-pod-struct-test

test: $(programs)



benchmark: hdf5-fixed-length-bench

# compiler assisted introspection will scan TU translation unit, which is the 
# collection of files you're compiling into an object, generates desired output
# from topologically ordered dependency graph. 
# currently POD structs, arrays and integral types are supported, in arbitrary deep
# embedding
#
# with -Dmyfile.h specifies the generated output, which must be sandwiched between
# <h5cpp/core> and <h5cpp/io>
generated.h: non-pod-struct.hpp
	h5cpp  non-pod-struct.cpp -- $(CXXFLAGS) $(INCLUDES) -Dgenerated.h

#non-pod-struct.o : non-pod-struct.cpp generated.h 
#	$(CXX) $(INCLUDES) -o non-pod-struct.o  $(CXXFLAGS) -c non-pod-struct.cpp

%.o : %.cpp 
	$(CXX) $(INCLUDES) -o $@  $(CPPFLAGS) $(CXXFLAGS) -c $^


#non-pod-struct: non-pod-struct.o
#	$(CXX) $^ $(LIBS) -o $@

%-test: %-test.o 
	$(CXX) $^ $(LIBS) -o $@
	taskset 0x1 ./$@

%-bench: %-bench.o 
	$(CXX) $^ $(LIBS) -o $@


clean:
	@$(RM) *.o *.h5 *-test *-bench
	
#non-pod-struct generated.h


.PHONY: test
