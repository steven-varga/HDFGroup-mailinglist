#  Copyright (c) Feb 18, 2021 Steven Varga, Toronto,ON Canada

CXXFLAGS =  -std=c++17 -O3  -I/usr/local/include 
LIBS = -lprofiler -lhdf5  -lz -ldl -lm 

test: ocpyr ocpyp
	./ocpyr --input original.h5 --source / --output copyr.h5 -d new_dir 
	./ocpyp --input original.h5 --source / --output copyp.h5 -d new_dir

%.o : %.cpp 
	$(CXX) -o $@  $(CPPFLAGS) $(CXXFLAGS) -c $^

ocpyr: ocpyr.o
	$(CXX) $^ $(LIBS) -o $@	

ocpyp: ocpyp.o
	$(CXX) $^ $(LIBS) -o $@	

clean:
	@$(RM) *.o $(apps) ocpy? copy?.h5 

dist-clean: clean
.PHONY: test

