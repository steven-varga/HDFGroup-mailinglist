#  _____________________________________________________________________________
#  Copyright (c) <2018> <copyright Steven Varga, Toronto, On>
#  Contact: Steven Varga
#           steven@vargaconsulting.ca
#           2018 Toronto, On Canada
#  _____________________________________________________________________________

CXXFLAGS =  -std=c++17
INCLUDES = -I/usr/local/include -I./include
LIB = -L/usr/local/lib/

send-f90: send.f90
	$(FC)  -Wall $(INCLUDES) $(LIB) -o send-f90 send.f90 -lfzmq -lzmq

send-cpp: send.cpp
	$(CXX)  -Wall $(INCLUDES) $(LIB) -o send-cpp send.cpp -lzmq

recv: recv.cpp
	$(CXX) $(INCLUDES) -o recv  $(CXXFLAGS) recv.cpp -lfzmq -lzmq -lhdf5 -lz -ldl -lm 


clean:
	@$(RM) *.o *.h5 $(apps)  struct

.PHONY: test

