#  Copyright (c) Feb 18, 2021 Steven Varga, Toronto,ON Canada

CXXFLAGS =  -std=c++17 -O3  -I/usr/local/include 
LIBS = -lprofiler -lhdf5  -lz -ldl -lm 

test: ocpyp ocpyr

test-ocpyr: ocpyr
	./ocpyr --input original.h5 --source / --output copyr.h5 -d new_dir 

test-ocpyp: ocpyp
	./ocpyp --input original.h5 --source / --output copyp.h5 -d new_dir




%.o : %.cpp 
	$(CXX) -o $@  $(CPPFLAGS) $(CXXFLAGS) -c $^

ocpyr: ocpyr.o
	$(CXX) $^ $(LIBS) -o $@	

ocpyp: ocpyp.o
	$(CXX) $^ $(LIBS) -o $@	

clean:
	@$(RM) *.o $(apps) ocpy? copy?.h5 ocpy?.callgrind ocpy?.prof  

cachegrind-ocpyr: ocpyr ocpyr.callgrind
	kcachegrind $*.callgrind

cachegrind-ocpyp: ocpyp ocpyp.callgrind
	kcachegrind $*.callgrind

%.callgrind:
	time ./$* --input /opt/rts.h5 --source / --output /tmp/remove-me-$*.h5 -d new_dir
	ls -lh  /tmp/remove-me-$*.h5
	@google-pprof --callgrind ./$* ./$*.prof > $*.callgrind
	#@google-pprof --text $* $*.prof

dist-clean: clean
.PHONY: test

