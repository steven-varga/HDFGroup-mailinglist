#  _____________________________________________________________________________
#  Copyright (c) <2022> <copyright Steven Varga, Toronto, On>
#  _____________________________________________________________________________

CXX=g++-10
CXXFLAGS =  -std=c++20
LIBS =  -lhdf5  -lz -ldl -lm -lpthread
INCLUDES = -I/usr/local/include -I/usr/include -I./include

test: scope
	./scope

hyperslab.o : hyperslab.cpp 
	$(CXX) $(INCLUDES) -o hyperslab.o  $(CXXFLAGS) -c hyperslab.cpp

hyperslab: hyperslab.o
	$(CXX) $^ $(LIBS) -o $@	

scope.o : scope.cpp 
	$(CXX) $(INCLUDES) -o scope.o  $(CXXFLAGS) -c scope.cpp

scope: scope.o
	$(CXX) $^ $(LIBS) -o $@	

clean:
	@$(RM) *.o *.h5 hyperslab

distclean: clean
.PHONY: test

