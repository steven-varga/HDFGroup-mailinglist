#  Copyright (c) apr 06, 2022 Steven Varga, Toronto,ON Canada

PKG_CFLAGS := $(shell pkg-config --cflags hdf5)
PKG_LIBS   := $(shell pkg-config --libs hdf5)

CXXFLAGS = -std=c++23 $(PKG_CFLAGS)
LIBS     = $(PKG_LIBS) -lz
INCLUDES = -I ./include

test: amr2h5
	./amr2h5

amr2h5: amr2h5.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $< -o $@ $(LIBS)

%.o : %.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $< -o $@

%-test: %-test.o
	$(CXX) $^ $(LIBS) -o $@

clean:
	@$(RM) *.o *.h5 *-test *.callgrind *.prof amr2h5

.PHONY: test clean dist-clean

dist-clean: clean