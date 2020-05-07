#include <gperftools/profiler.h>
#include <cstdint>
#include "struct.h"
#include <h5cpp/core>
	#include "generated.h"
#include <h5cpp/io>
#include <chrono>


int main(int argc, char **argv) {

	h5::fd_t fd = h5::create("example.h5",H5F_ACC_TRUNC);
    {
        sn::log x; x.idx=0;
        h5::pt_t pt; // initial condition: H5I_UNINIT
        std::cout << pt;

        pt = h5::create<sn::log>(fd, "log.000",
                h5::max_dims{ H5S_UNLIMITED }, h5::chunk{ 2 }  );
        std::cout << pt;
            h5::append(pt, x ); x.idx++;  std::cout << pt;
            h5::append(pt, x ); x.idx++;  std::cout << pt;
            h5::append(pt, x ); x.idx++;  std::cout << pt;
        pt = h5::pt_t();

        pt = h5::create<sn::log>(fd, "log.001",
                h5::max_dims{ H5S_UNLIMITED }, h5::chunk{ 10 } | h5::gzip{9} );
            h5::append(pt, x ); x.idx++; //std::cout << pt;
            h5::append(pt, x ); x.idx++; //std::cout << pt;
            h5::append(pt, x ); x.idx++; //std::cout << pt;
            h5::append(pt, x ); x.idx++; //std::cout << pt;
            h5::append(pt, x ); x.idx++; //std::cout << pt;
            h5::append(pt, x ); x.idx++; //std::cout << pt;
            h5::append(pt, x ); x.idx++; //std::cout << pt;
            h5::append(pt, x ); x.idx++; //std::cout << pt;           
            h5::append(pt, x ); x.idx++; //std::cout << pt;
            h5::append(pt, x ); x.idx++; std::cout << pt;
            // flush happens here
            h5::append(pt, x ); x.idx++; //std::cout << pt;
            h5::append(pt, x ); x.idx++; std::cout << pt;
        pt = h5::create<sn::log>(fd, "log.002",  // <-- move assign will trigger flush
                h5::max_dims{ H5S_UNLIMITED }, h5::chunk{ 10 } | h5::gzip{9} );
            h5::append(pt, x ); x.idx++; //std::cout << pt;
            h5::append(pt, x ); x.idx++; std::cout << pt;
    }
    {

        sn::log x; x.idx=0;
        long long size=0;
        h5::pt_t pt;
		std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
     //   ProfilerStart( (std::string(argv[0]) + std::string(".prof")).data() );
        for (int i=0; i < 10'000; i++){
            pt = h5::create<sn::log>(fd, "log." + std::to_string(i),
                h5::max_dims{ H5S_UNLIMITED }, h5::chunk{ 1024 }  );
            for (int j=0; j<10'000; j++) size++, x.idx = j, h5::append(pt, x );
        }
     //   ProfilerStop();
        std::chrono::system_clock::time_point stop = std::chrono::system_clock::now();
		double running_time = 1e-6 * std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
		std::cout << "record size: " << sizeof(sn::log) << "\n";
		std::cout << "number of mu seconds: " << running_time << " record per sec: " << size/running_time
	 		<< " sustained throughput: " << ((size/running_time)*sizeof(sn::log )) / 1'000'000 <<" Mbyte/sec"	<< std::endl;
		std::cout << "transferred data: " << 1e-6 * size * sizeof(sn::log) << "Mbyte\n";
    }
	return 0;
}
