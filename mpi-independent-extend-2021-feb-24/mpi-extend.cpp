/*
steven varga, 2021, feb 22, Toronto, ON, Canada;  MIT license
*/

#include "argparse.h"
#include <mpi.h>  /* MUST preceede H5CPP includes*/ 
#include <h5cpp/all>
#include <string>
#include <vector>
#include <thread>
#include <random>
#include <chrono>


// BEING SETUP: we are to create RANK many independent datasets with random sizes
// with colective call
int main(int argc, char **argv) {

   	// usual boiler place
    int rank_size, current_rank, name_len;
    char processor_name[MPI_MAX_PROCESSOR_NAME], dataset_name[1000];
    MPI_Init(NULL, NULL);
	MPI_Info info  = MPI_INFO_NULL;
	MPI_Comm comm  = MPI_COMM_WORLD;

    MPI_Comm_size(MPI_COMM_WORLD, &rank_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);
    MPI_Get_processor_name(processor_name, &name_len); 
    
    // obtain processor and rank dependent name:
    std::mt19937_64 eng{std::random_device{}()};  // or seed however you want
    std::uniform_int_distribution<> some_job{10, 50}, some_dims{100,1000};

    try {
        std::vector<size_t> nrows(rank_size);
        std::generate(std::begin(nrows), std::end(nrows), [&](){
            return some_dims(eng);
        });
        // all process must have the same dimensions
        MPI_Bcast( nrows.data(), rank_size, MPI_INT64_T, 0, MPI_COMM_WORLD);

        //we can have different data per rank
        std::vector<double> v(nrows[current_rank]);
        std::fill(std::begin(v), std::end(v), current_rank);
        
        // open file with MPIIO
        h5::fd_t fd = h5::create("mpi-extend.h5", H5F_ACC_TRUNC,
                h5::fcpl, h5::mpiio({MPI_COMM_WORLD, info})); // pass the communicator and hints as usual
        // simulate random job to cause process divergence
        std::this_thread::sleep_for(std::chrono::milliseconds{10000});
        for(int i=0; i<rank_size; i++) { 

            sprintf(dataset_name, "%s-%02d", processor_name, i);
            // you don't know which process will call the actual H5Dcreate(...)
            h5::create<double>( fd, dataset_name,
                h5::chunk{nrows[i],1}, h5::current_dims{nrows[i], 0}, h5::max_dims{nrows[i], H5S_UNLIMITED});
        }
        // simulate random job
        std::this_thread::sleep_for(std::chrono::milliseconds{some_job(eng)});
// END SETUP


// NOTE:
// the following line last integer entry, currently set to zero (0) controls
// of the datasets are same on ALL PROCESSES, setting this to `current_rank` 
// will trying to extend EACH datasets independently, and will HANG
        sprintf(dataset_name, "%s-%02d", processor_name, 0);
        h5::ds_t ds = h5::open(fd, dataset_name);
        h5::sp_t sp = h5::get_space(ds); // only creates the space, no IO ops called
        // iostream operator `<< h5::sp_t` queries the dimensions, and outputs it for you
        // they all have different first dimension, and 0 last
        std::cout << sp;

        /*NOTE: at this point the second rank is zero!!! */
        // use mix of H5CPP and C API calls:
        h5::current_dims current_dims;
        h5::get_simple_extent_dims(sp, current_dims);
        // this is the current dimensions for `this` rank
        std:: cout << current_dims;
        current_dims[1]  = 400;
// NOTE:
// uncommenting to following line is logically same as the previous comment explains
// will result in hanging process         
        //if(current_rank ==0)
            H5Dset_extent(ds, *current_dims);

    } catch ( const h5::error::any& e ) {
        std::cerr << e.what() << std::endl;
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
	return 0;
}