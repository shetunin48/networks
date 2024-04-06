#!/bin/sh
make -s
./server_client.o 8080 -p UDP
make clean -s
