#!/bin/sh
make -s
./server_client.o 8080 8081 -p UDP
make clean -s
