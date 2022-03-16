#  _____________________________________________________________________________
#  Copyright (c) <2022> <copyright Steven Varga, Toronto, On>
#  _____________________________________________________________________________

CXXFLAGS =  -std=c++17
LIBS =  -lhdf5  -lz -ldl -lm 
INCLUDES = -I/usr/local/include -I/usr/include  -I./
test: custom opaque
	./custom
	./opaque

custom.o : custom.cpp custom.hpp 
	$(CXX) $(INCLUDES) -o custom.o  $(CXXFLAGS) -c custom.cpp

custom: custom.o
	$(CXX) $^ $(LIBS) -o $@	

opaque.o : opaque.cpp opaque.hpp
	$(CXX) $(INCLUDES) -o opaque.o  $(CXXFLAGS) -c opaque.cpp

opaque: opaque.o
	$(CXX) $^ $(LIBS) -o $@	

clean:
	@$(RM) *.o *.h5 custom opaque

dist-clean: clean
.PHONY: test

