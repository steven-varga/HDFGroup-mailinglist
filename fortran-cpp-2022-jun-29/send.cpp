#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

int main (void){
    void *ctx = zmq_ctx_new ();
    void *sock = zmq_socket (ctx, ZMQ_PUSH);
    zmq_connect (sock, "tcp://localhost:5555");

    for(uint64_t i=0; i<100; i++)
        zmq_send (sock, &i, 8, 0);

    zmq_close (sock);
    zmq_ctx_destroy (ctx);
    return 0;
}