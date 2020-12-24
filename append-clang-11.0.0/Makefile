#  _________________________________________________________
#  Copyright (c) 2018-2020 Steven Varga, Toronto,ON Canada
#  Author: Varga, Steven <steven@vargaconsulting.ca>
#  _________________________________________________________

CXXFLAGS =  -std=c++17 -O3  -I/usr/local/include 
LIBS = -lhdf5  -lz -ldl -lm 
CXX=clang++

test: packettable
	./packettable
	h5dump  example.h5 

packettable.o : packettable.cpp 
	$(CXX)  -o packettable.o  $(CXXFLAGS) -c packettable.cpp

packettable: packettable.o
	$(CXX) $^ $(LIBS) -o $@	

clean:
	@$(RM) *.o *.h5 $(apps)  packettable 

dist-clean: clean
.PHONY: test

