#  _____________________________________________________________________________
#  Copyright (c) <2018> <copyright Steven Varga, Toronto, On>
#  Contact: Steven Varga
#           steven@vargaconsulting.ca
#           2018 Toronto, On Canada
#  _____________________________________________________________________________

CXXFLAGS =  -std=c++17
LIBS =  -lhdf5  -lz -ldl -lm
INCLUDES = -I./include

test: struct
	./struct
	h5dump -pH test.h5

# compiler assisted introspection will scan TU translation unit, which is the 
# collection of files you're compiling into an object, generates desired output
# from topologically ordered dependency graph. 
# currently POD structs, arrays and integral types are supported, in arbitrary deep
# embedding
#
# with -Dmyfile.h specifies the generated output, which must be sandwiched between
# <h5cpp/core> and <h5cpp/io>
generated.h: struct.h
	h5cpp  struct.cpp -- $(CXXFLAGS) $(INCLUDES) -I/usr/include/h5cpp-llvm -Dgenerated.h

struct.o : struct.cpp 
	$(CXX) $(INCLUDES) -o struct.o  $(CXXFLAGS) -c struct.cpp

struct: struct.o
	$(CXX) $^ $(LIBS) -o $@	

clean:
	@$(RM) *.o *.h5 $(apps)  struct

dist-clean: clean
	rm generated.h 

.PHONY: test

