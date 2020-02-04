# problem: how to save many small armadillo matrices efficiently
For 400 iterations (100 Mb) it takes 0.15 seconds
For 4000 iterations (1 Gb) it varies a lot: from 8 to 16 seconds (I dont know why)
According to my SSD information it has the I/O speed 500 Mb/sec

I’m going to try the same experiment with hyperslab and H5CPP


# Solution
```
g++ -I/usr/include -o main.o  -std=c++17 -c main.cpp
g++ main.o -lhdf5  -lz -ldl -lm -larmadillo -o main	
./main
CREATING 4000 h5::ds_t cost:0.215566 or 18555.8dataset / sec
GENERATING  4000 random matrices,  cost:30.9288 or 129.329matrix / sec
WRITING  4000 random matrices to dataset,  cost:1.46594 or 2728.62matrix / sec
THROUHPUT: 687.613MB/s#h5dump -pH container.h5
```


