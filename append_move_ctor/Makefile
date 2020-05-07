#  _________________________________________________________
#  Copyright (c) 2018-2020 Steven Varga, Toronto,ON Canada
#  Author: Varga, Steven <steven@vargaconsulting.ca>
#  _________________________________________________________

CXXFLAGS =  -std=c++17 -O3  -I/usr/local/include 
LIBS = -lprofiler -lhdf5 -lhdf5_hl  -lz -ldl -lm 

test: packettable
	./packettable
	h5dump -pH example.h5 | grep -c log

generated.h: struct.h
	h5cpp  packettable.cpp -- $(CXXFLAGS)  -Dgenerated.h

packettable.o : packettable.cpp generated.h 
	$(CXX)  -o packettable.o  $(CXXFLAGS) -c packettable.cpp

packettable: packettable.o
	$(CXX) $^ $(LIBS) -o $@	


maksim.o : maksim.cpp generated.h 
	$(CXX) -o maksim.o  $(CXXFLAGS) -c maksim.cpp

maksim: maksim.o
	$(CXX) $^ $(LIBS) -o $@	

cachegrind: packettable
	valgrind --tool=cachegrind --cachegrind-out-file=packettable.cache ./packettable
	cg_annotate packettable.cache >> result
	kcachegrind packettable.cache

valgrind: packettable
	valgrind ./packettable

profile: packettable
	./packettable
	google-pprof --callgrind ./packettable ./packettable.prof > packettable.callgrind
	google-pprof --text packettable packettable.prof
	kcachegrind packettable.callgrind &>/dev/null



clean:
	@$(RM) *.o *.h5 $(apps)  packettable 

dist-clean: clean
	@$(RM) generated.h
.PHONY: test

