
CXXFLAGS =  -std=c++17
LIBS =  -lhdf5  -lz -ldl -lm
INCLUDES = -I/usr/local/include/highfive

test: high5 h5cpp
	./h5cpp
	./high5

high5.o: high5.cpp
	$(CXX) $(INCLUDES) -o high5.o  $(CXXFLAGS)  -c high5.cpp

high5: high5.o
	$(CXX) $^ $(LIBS) -o $@	

h5cpp.o: h5cpp.cpp
	$(CXX) $(INCLUDES) -o h5cpp.o  $(CXXFLAGS)  -c h5cpp.cpp

h5cpp: h5cpp.o
	$(CXX) $^ $(LIBS) -o $@


clean:
	@$(RM) *.o *.h5 h5cpp high5 


.PHONY: test

