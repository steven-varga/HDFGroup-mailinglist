#  _____________________________________________________________________________
#  Copyright (c) <2018> <copyright Steven Varga, Toronto, On>
#  Contact: Steven Varga
#           steven@vargaconsulting.ca
#           2018 Toronto, On Canada
#  _____________________________________________________________________________

CXXFLAGS =  -std=c++17
INCLUDES = -I/usr/local/include -I./include

send: send.f90
	gfortran-11 -Wall $(INCLUDES) -L/usr/local/lib/ -o send send.f90 -lfzmq -lzmq

recv: recv.cpp
	$(CXX) $(INCLUDES) -o recv  $(CXXFLAGS) recv.cpp -lfzmq -lzmq -lhdf5 -lz -ldl -lm 


clean:
	@$(RM) *.o *.h5 $(apps)  struct

.PHONY: test

