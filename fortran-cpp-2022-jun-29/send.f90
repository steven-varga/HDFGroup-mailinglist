! https://github.com/richsnyder/fzmq
! send.f90
program main
    use, intrinsic  :: iso_c_binding
    use             :: zmq
    type(c_ptr)     :: ctx, sock
    type(zmq_msg_t) :: msg
    integer         :: n, rc
    integer(8), target :: payload = 0

    ctx = zmq_ctx_new()
    sock = zmq_socket(ctx, ZMQ_PUSH)
    rc = zmq_connect(sock, 'tcp://localhost:5555')
    do i=1, 100, 1
        payload = i
        rc = zmq_msg_init_data(msg, c_loc(payload), c_sizeof(payload), c_null_ptr, c_null_ptr)
        n = zmq_msg_send(msg, sock, 0)
    end do
    rc = zmq_close(sock)
    rc = zmq_ctx_term(ctx)
end program main
