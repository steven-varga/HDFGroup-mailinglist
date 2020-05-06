#  _________________________________________________________
#  Copyright (c) 2018-2020 Steven Varga, Toronto,ON Canada
#  Author: Varga, Steven <steven@vargaconsulting.ca>
#  _________________________________________________________

CXXFLAGS =  -std=c++17 -O3  -I/usr/local/include -I/usr/include/eigen3 
LIBS =  -lhdf5 -lhdf5_hl  -lz -ldl -lm -lblas -llapack

test: packettable
	./packettable
	h5dump -pH example.h5 | grep -c log

generated.h: struct.h
	h5cpp  packettable.cpp -- $(CXXFLAGS)  -Dgenerated.h

packettable.o : packettable.cpp generated.h 
	$(CXX) -I/usr/include/eigen3 -o packettable.o  $(CXXFLAGS) -c packettable.cpp

packettable: packettable.o
	$(CXX) $^ $(LIBS) -o $@	

clean:
	@$(RM) *.o *.h5 $(apps)  packettable 

dist-clean: clean
	@$(RM) generated.h
.PHONY: test

