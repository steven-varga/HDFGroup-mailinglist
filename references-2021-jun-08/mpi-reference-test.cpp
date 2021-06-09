/* copyright steven varga, vargaconsulting 2021, june 08, Toronto, ON, Canada;  MIT license
*/

#include <mpi.h>  /* MUST preceede H5CPP includes*/ 
#include <h5cpp/all>
#include <string>
#include <vector>
#include <fmt/format.h>


constexpr hsize_t m=1489, n=2048, k=2;
constexpr const char* filename = "mpi-reference.h5";
constexpr float data_value = 3.0;

int main(int argc, char **argv) {

    int rank_size, current_rank, name_len;
    MPI_Init(NULL, NULL);
	MPI_Info info  = MPI_INFO_NULL;
	MPI_Comm comm  = MPI_COMM_WORLD;

    MPI_Comm_size(MPI_COMM_WORLD, &rank_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);
    
    if(current_rank == 0) { // serial mode, ran on rank 0
        h5::fd_t fd = h5::create(filename, H5F_ACC_TRUNC);
        for(int i=0; i<rank_size; i++){
            // this is your dataset:
            h5::ds_t ds = h5::create<double>(fd, fmt::format("{:03d}/dataset", i), h5::chunk{149,256,1}, h5::current_dims{m,n,k});
            // VL string attribute written to it
            ds["attribute name"] = "this is a vl string attribute";
            // and a reference, with rank_size -- which is 10 in the description
            h5::create<h5::reference_t>(fd, fmt::format("{:03d}/reference", i), h5::current_dims{static_cast<hsize_t>(rank_size)}, 
                h5::chunk{2});
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);

    { // parallel mode
        h5::fd_t fd = h5::open(filename, H5F_ACC_RDWR, h5::mpiio({MPI_COMM_WORLD, info}));
        h5::write(fd,  fmt::format("{:03d}/dataset",current_rank), 
            std::vector<float>(m*n*k, 1.0), // <-- single shot write: dataset size must match file space size 
            h5::independent);               // collective | independent
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if(current_rank == 0) { // serial mode, ran on rank 0
        // dimension >= referenced region or bad things happen
        std::vector<float> in_memory_values(64*64, data_value);

        h5::fd_t fd = h5::open(filename, H5F_ACC_RDWR);
        for(int i=0; i<rank_size; i++){
            h5::reference_t reference = h5::reference(fd, // CREATE a reference
                fmt::format("{:03d}/dataset", i),         // dataset the region is within
                h5::offset{1, 1, 0},                      // location of the region 
                h5::count{64,64,1});                      // size of region 
            // write data into the referenced region, the memepry space of passed pointer must match the region 
            h5::exp::write(fd, fmt::format("{:03d}/dataset", i) , reference, in_memory_values.data());
            // don't forget to write the REFERENCE into the index file for later use,
            // NOTE: the order of data and reference IO calls is arbitrary
            h5::write(fd, fmt::format("{:03d}/reference", i), 
                reference, 
                h5::count{1}, h5::offset{0} ); // <-- which cell you update within file space
        };
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    
	return 0;
}