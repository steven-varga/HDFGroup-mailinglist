#  Copyright (c) apr 06, 2022 Steven Varga, Toronto,ON Canada

CXXFLAGS =  -std=c++17 -DFMT_HEADER_ONLY
LIBS =  -lhdf5  -lz -ldl -lm 
INCLUDES = -I./include

test: pprint-test
	./pprint-test

%.o : %.cpp 
	$(CXX) $(INCLUDES) -o $@  $(CPPFLAGS) $(CXXFLAGS) -c $^

%-test: %-test.o 
	$(CXX) $^ $(LIBS) -o $@

clean:
	@$(RM) *.o *.h5 *-test  *.callgrind *.prof  
	
.PHONY: test

dist-clean: clean
.PHONY: test
