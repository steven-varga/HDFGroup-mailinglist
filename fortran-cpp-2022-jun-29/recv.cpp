/* Copyright (c) 2022 vargaconsulting, Toronto,ON Canada */

#include <cstdint>
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <h5cpp/all>

int main() {

	h5::fd_t fd = h5::create("collected-data.h5", H5F_ACC_TRUNC);
    void *ctx = zmq_ctx_new ();
    void *sock = zmq_socket (ctx, ZMQ_PULL);
    int rc = zmq_bind (sock, "tcp://*:5555");

	// we're roughly controlling IO caches with chunk size, technically you want it about 
	// to be the underlying buffer size: 1MB for jumbo ethernet frames, 64Kb for low latency interconnects
	h5::pt_t pt = h5::create<int64_t>(fd, "some channel xyz",
			h5::max_dims{H5S_UNLIMITED}, h5::chunk{1024});

	int64_t buffer, count=100;
	while(count--)
        if( zmq_recv (sock, &buffer, sizeof(int64_t), 0) >= 0)
			h5::append( pt, buffer);

	return 0;
}