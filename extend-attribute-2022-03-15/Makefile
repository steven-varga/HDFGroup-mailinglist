
CXXFLAGS =  -std=c++17
LIBS =  -lhdf5  -lz -ldl -lm

test: h5cpp
	./h5cpp
	h5dump -a /some_dataset/attribute_name  h5cpp.h5

h5cpp.o: h5cpp.cpp
	$(CXX) $(INCLUDES) -o h5cpp.o  $(CXXFLAGS)  -c h5cpp.cpp

h5cpp: h5cpp.o
	$(CXX) $^ $(LIBS) -o $@


clean:
	@$(RM) *.o *.h5 h5cpp 


.PHONY: test

