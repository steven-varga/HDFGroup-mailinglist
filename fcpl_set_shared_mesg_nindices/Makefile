#  _____________________________________________________________________________
#  Copyright (c) <2018> <copyright Steven Varga, Toronto, On>
#  Contact: Steven Varga
#           steven@vargaconsulting.ca
#           2018 Toronto, On Canada
#  _____________________________________________________________________________

LIBS =  -lhdf5  -lz -ldl -lm
INCLUDES = -I/usr/include
app = set_shared_mesg_nindices

test: clean compile
	./$(app)

compile:
	$(CC) main.c $(LIBS) -o $(app)	

clean:
	@$(RM) *.o *.h5 $(app)

.PHONY: test

