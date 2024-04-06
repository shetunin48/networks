#!/bin/sh
make -s
./server_client.o 8080 -p TCP --client 127.0.0.1
make clean -s
