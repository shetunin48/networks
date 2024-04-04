#!/bin/sh
make -s
./server_client.o 8080 -p TCP --client 8081
make clean -s
