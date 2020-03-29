#  _____________________________________________________________________________
#  Copyright (c) <2020> <copyright Steven Varga, Toronto, On>
#  _____________________________________________________________________________

src = ConversionTests.c

all: H5_LDOUBLE_TO_LONG_SPECIAL_TEST  \
	H5_LONG_TO_LDOUBLE_SPECIAL_TEST   \
	H5_LDOUBLE_TO_LLONG_ACCURATE_TEST \
	H5_LLONG_TO_LDOUBLE_CORRECT_TEST  \
	H5_NO_ALIGNMENT_RESTRICTIONS_TEST \
	H5_DISABLE_SOME_LDOUBLE_CONV_TEST

# generate output as function of -DH5_xyz
% : $(src)
	@$(CC) -o $@ -D$@  $(src) 
	@./$@ ; echo $@ $$? 

clean:
	@$(RM) H5*

.PHONY: all
