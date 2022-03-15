# [Problem](https://forum.hdfgroup.org/t/a-problem-when-saving-native-ldouble-variables/9504)
I’m new to hdf5. I have tried to used it to save the results of some simulations. However, everything was very good until I wanted to save “long double” variables. When saving “double” there is no problem. But when I use “long double”, the numbers saved in the hdf5 file fluctuate between very large and very small values. Surely, they are not the correct results of the simulation.

My question: how to properly save “long double” variables knowing that the results are wrong when mapped to NATIVE_LDOUBLE?

# Solution
H5CPP support LONG DOUBLE out of the box, however `h5dump` prints out the wrong values

```
h5dump -d /homogenious test.h5

DATASET "homogenious" {
   DATATYPE  H5T_NATIVE_LDOUBLE
   DATASPACE  SIMPLE { ( 5, 3 ) / ( 5, 3 ) }
   DATA {
   (0,0): 4.94066e-324, 4.94066e-324, 4.94066e-324,
   (1,0): 4.94066e-324, 4.94066e-324, 4.94066e-324,
   (2,0): 4.94066e-324, 4.94066e-324, 4.94066e-324,
   (3,0): 4.94066e-324, 4.94066e-324, 4.94066e-324,
   (4,0): 4.94066e-324, 4.94066e-324, 4.94066e-324
   }
}
```
however reading back the dataset, you get the correct vvalues:

```
homogenious      : -0.42 -0.42 -0.42 -0.42 -0.42 -0.42 -0.42 -0.42 -0.42 -0.42 -0.42 -0.42 -0.42 -0.42 -0.42 
record type(temp): 0 0.01 0.02 0.03 0.04 0.05 0.06 0.07 0.08 0.09 
```


```
g++ -I/usr/include -o longdouble.o  -std=c++17 -c longdouble.cpp
g++ longdouble.o -lhdf5  -lz -ldl -lm -o longdouble	
./longdouble
0 0.01 0.02 0.03 0.04 0.05 0.06 0.07 0.08 0.09 
h5dump -pH test.h5
HDF5 "test.h5" {
GROUP "/" {
   DATASET "homogenious" {
      DATATYPE  H5T_NATIVE_LDOUBLE
      DATASPACE  SIMPLE { ( 5, 3 ) / ( 5, 3 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 1, 3 )
         SIZE 0
      }
      FILTERS {
         NONE
      }
      FILLVALUE {
         FILL_TIME H5D_FILL_TIME_IFSET
         VALUE  4.94066e-324
      }
      ALLOCATION_TIME {
         H5D_ALLOC_TIME_INCR
      }
   }
   DATASET "stream-of-records" {
      DATATYPE  H5T_COMPOUND {
         H5T_NATIVE_LDOUBLE "temp";
         H5T_NATIVE_LDOUBLE "density";
         H5T_ARRAY { [3] H5T_NATIVE_LDOUBLE } "B";
         H5T_ARRAY { [3] H5T_NATIVE_LDOUBLE } "V";
         H5T_ARRAY { [20] H5T_NATIVE_LDOUBLE } "dm";
         H5T_ARRAY { [9] H5T_NATIVE_LDOUBLE } "jkq";
      }
      DATASPACE  SIMPLE { ( 10 ) / ( H5S_UNLIMITED ) }
      STORAGE_LAYOUT {
         CHUNKED ( 512 )
         SIZE 634 (9.338:1 COMPRESSION)
      }
      FILTERS {
         COMPRESSION DEFLATE { LEVEL 9 }
      }
      FILLVALUE {
         FILL_TIME H5D_FILL_TIME_IFSET
         VALUE  H5D_FILL_VALUE_DEFAULT
      }
      ALLOCATION_TIME {
         H5D_ALLOC_TIME_INCR
      }
   }
}
}
```
