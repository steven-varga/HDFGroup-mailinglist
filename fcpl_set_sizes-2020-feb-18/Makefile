#  _____________________________________________________________________________
#  Copyright (c) <2018> <copyright Steven Varga, Toronto, On>
#  Contact: Steven Varga
#           steven@vargaconsulting.ca
#           2018 Toronto, On Canada
#  _____________________________________________________________________________

LIBS =  -lhdf5  -lz -ldl -lm
INCLUDES = -I/usr/include

test: clean set_sizes
	./set_sizes

set_sizes:
	$(CC) main.c $(LIBS) -o set_sizes	

clean:
	@$(RM) *.o *.h5 set_sizes

.PHONY: test

