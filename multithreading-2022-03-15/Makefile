#  _____________________________________________________________________________
#  Copyright (c) <2022> <copyright Steven Varga, Toronto, On>
#  _____________________________________________________________________________

CXX=g++-10
CXXFLAGS =  -std=c++20
LIBS =  -lhdf5  -lz -ldl -lm -lpthread
INCLUDES = -I/usr/local/include -I/usr/include -I./

test: threads
	./threads

threads.o : threads.cpp queue.hpp utils.hpp
	$(CXX) $(INCLUDES) -o threads.o  $(CXXFLAGS) -c threads.cpp

threads: threads.o
	$(CXX) $^ $(LIBS) -o $@	

clean:
	@$(RM) *.o *.h5 threads

distclean: clean
.PHONY: test

