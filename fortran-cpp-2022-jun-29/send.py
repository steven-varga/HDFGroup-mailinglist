#!/usr/bin/python3

import zmq

context = zmq.Context()
sock = context.socket(zmq.PUSH)
sock.connect("tcp://localhost:5555")

for x in range(100):
    sock.send(x.to_bytes(8, 'little'))