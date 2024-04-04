#include <arpa/inet.h>
#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <sys/wait.h>
   
int main(int argc, char const* argv[]) 
{
    int type = SOCK_STREAM;
    int protocol = 0;
    for(int i = 1; i < argc - 1; ++i) {
        if(strcmp("-h", argv[i]) == 0 || strcmp("--help", argv[i]) == 0) {
            printf("This is client and server program in one. To use it you should \
                    specify PORT1 which is server port, and PORT2 which is client \
                    port (they could be equal). You can also specify -p or --protocol \
                    for \"UDP\" or \"TCP\" (default is TCP).\n\n");
            exit(0);
        }
        if(strcmp("-p", argv[i]) == 0 || strcmp("--protocol", argv[i]) == 0) {
            if(strcmp("UDP", argv[i + 1]) == 0) {
                type = SOCK_DGRAM;
            }
            else if(strcmp("TCP", argv[i + 1]) != 0) {
                printf("Unknown protocol\n");
                exit(0);
            }
        }
    }
    if(argc < 3) {
        printf("Usage: ./%s PORT1 PORT2\n", argv[0]);
        exit(0);
    }
    int PORT1, PORT2;
    sscanf(argv[1], "%d", &PORT1);
    sscanf(argv[2], "%d", &PORT2);

    int sockfd_serv, sockfd_cli; 
   
    sockfd_serv = socket(AF_INET, type, protocol); 
    sockfd_cli = socket(AF_INET, type, protocol); 
    if (sockfd_serv < 0 || sockfd_cli < 0) {
        perror("ERROR socket");
        exit(-1); 
    }

    struct sockaddr_in server, client;

    memset(&server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET; 
    server.sin_port = htons(PORT1);
    server.sin_addr.s_addr = INADDR_ANY;

    memset(&client, 0, sizeof(struct sockaddr_in));
    client.sin_family = AF_INET; 
    client.sin_port = htons(PORT2);
    client.sin_addr.s_addr = INADDR_ANY;

    int option = 1;
    setsockopt(sockfd_serv, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    if (bind(sockfd_serv, (struct sockaddr *)&server, sizeof(server)) != 0) { 
        perror("ERROR bind");
        close(sockfd_serv); 
        close(sockfd_cli); 
        exit(-1); 
    }
    if(type == SOCK_STREAM) {
        if(fork()) { // server TCP
            int len = sizeof(struct sockaddr);
            int connfd_in;
            if ((listen(sockfd_serv, 5)) != 0) { 
                perror("ERROR listen");
                close(sockfd_serv); 
                close(sockfd_cli);  
                exit(-1); 
            }
            printf("I'm listenning\n");
            if ((connfd_in = accept(sockfd_serv, (struct sockaddr *)&client, &len)) < 0) { 
                perror("ERROR accept");
                close(sockfd_serv); 
                close(sockfd_cli); 
                exit(-1); 
            }
            char s[256];

            write(connfd_in, "Hello, I'm server!", 19);
            read(connfd_in, s, 255);
            printf("I'm server, I recieved \"%s\"\n", s);
            close(connfd_in);

            wait(NULL);
        }
        else { // client TCP
            if (connect(sockfd_cli, (struct sockaddr*)&server, sizeof(server)) < 0) {
                perror("ERROR connect");
                close(sockfd_serv); 
                close(sockfd_cli); 
                exit(-1); 
            }
            char s[256];

            write(sockfd_cli, "Hello, I'm client!", 19);
            read(sockfd_cli, s, 255);
            printf("I'm client, I recieved \"%s\"\n", s);
        }
    }
    if(type == SOCK_DGRAM) {
        if(fork()) { // server UDP
            char s[256];
            int len = sizeof(client);

            if (recvfrom(sockfd_serv, s, sizeof(s), 0, (struct sockaddr*)&client, &len) < 0){
                perror("ERROR recvfrom");
                close(sockfd_serv); 
                close(sockfd_cli); 
                exit(-1); 
            }
            printf("I'm server, I recieved \"%s\"\n", s);
            if (sendto(sockfd_serv, "Hello, I'm server!", 19, 0, (struct sockaddr*)&client, len) < 0){
                perror("ERROR sendto");
                close(sockfd_serv); 
                close(sockfd_cli); 
                exit(-1); 
            }

            wait(NULL);
        }
        else { // client UDP
            char s[256];
            int len = sizeof(server);

            if (sendto(sockfd_cli, "Hello, I'm client!", 19, 0, (struct sockaddr*)&server, len) < 0){
                perror("ERROR sendto");
                close(sockfd_serv); 
                close(sockfd_cli); 
                exit(-1); 
            }
            if (recvfrom(sockfd_cli, s, sizeof(s), 0, (struct sockaddr*)&server, &len) < 0){
                perror("ERROR recvfrom");
                close(sockfd_serv); 
                close(sockfd_cli); 
                exit(-1); 
            }
            printf("I'm client, I recieved \"%s\"\n", s);   

        }
    }
    
    close(sockfd_serv);
    close(sockfd_cli);
    return 0;
}