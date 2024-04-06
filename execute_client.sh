#!/bin/sh
make -s
./server_client.o 8080 -p UDP --client 127.0.0.1
make clean -s
