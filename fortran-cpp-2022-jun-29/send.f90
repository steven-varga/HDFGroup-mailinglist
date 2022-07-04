! MIT, steven varga, h5cpp.org
! send.f90
program main
    use, intrinsic  :: iso_c_binding
    use             :: zmq
    type(c_ptr)     :: ctx, sock
    integer(c_int) ::  res, rc
    integer(c_size_t), target :: i, size=8

    ! push - pull pattern
    ctx = zmq_ctx_new()
    sock = zmq_socket(ctx, ZMQ_PUSH)
    rc = zmq_connect(sock, 'tcp://localhost:5555')
    
    do i=1, 10**8
        res = zmq_send(sock, c_loc(i), size, 0)        
    end do
    ! in our simple data exchange `0x0` represents end of stream
    ! send close sgnal to `recv`
    i = 0
    res = zmq_send(sock, c_loc(i), size, 0)
    
    rc = zmq_close(sock)
    rc = zmq_ctx_term(ctx)
end program main
