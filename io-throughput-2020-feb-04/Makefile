#  _____________________________________________________________________________
#  Copyright (c) <2018> <copyright Steven Varga, Toronto, On>
#  Contact: Steven Varga
#           steven@vargaconsulting.ca
#           2018 Toronto, On Canada
#  _____________________________________________________________________________

CXXFLAGS =  -std=c++17
LIBS =  -lhdf5  -lz -ldl -lm -larmadillo
INCLUDES = -I/usr/include

test: main
	./main
	#h5dump -pH container.h5


main.o : main.cpp 
	$(CXX) $(INCLUDES) -o main.o  $(CXXFLAGS) -c main.cpp

main: main.o
	$(CXX) $^ $(LIBS) -o $@	

clean:
	@$(RM) *.o *.h5 main

dist-clean: clean

.PHONY: test

