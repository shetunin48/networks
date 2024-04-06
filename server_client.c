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
   
void fill_info(struct sockaddr_in* sock, int PORT, char *ip) {
    memset(sock, 0, sizeof(struct sockaddr_in));
    sock->sin_family = AF_INET; 
    sock->sin_port = htons(PORT);
    inet_aton(ip, (struct in_addr*)&sock->sin_addr.s_addr);
}

void TCPserver(int sockfd, struct sockaddr_in *client) {
    int len = sizeof(struct sockaddr);
    int connfd_in;
    if ((listen(sockfd, 1)) != 0) { 
        perror("ERROR listen");
        close(sockfd); 
        exit(-1); 
    }
    printf("I'm listenning TCP\n");
    if ((connfd_in = accept(sockfd, (struct sockaddr *)client, &len)) < 0) { 
        perror("ERROR accept");
        close(sockfd); 
        exit(-1); 
    }
    char s[256];

    read(connfd_in, s, 255);
    write(connfd_in, "Hello, I'm server!", 19);
    printf("I'm server, I recieved \"%s\"\n", s);
    close(connfd_in);
}

void TCPcli(int sockfd, struct sockaddr_in *server) {
    if (connect(sockfd, (struct sockaddr*)server, sizeof(*server)) < 0) {
        perror("ERROR connect");
        close(sockfd); 
        exit(-1); 
    }
    char s[256];

    write(sockfd, "Hello, I'm client!", 19);
    read(sockfd, s, 255);
    printf("I'm client, I recieved \"%s\"\n", s);
}

void UDPserver(int sockfd, struct sockaddr_in *client) {
    char s[256];
    int len = sizeof(*client);

    printf("I'm listenning UDP\n");
    if (recvfrom(sockfd, s, sizeof(s) - 1, 0, (struct sockaddr*)client, &len) < 0){ // Maybe here is overflow?)
        perror("ERROR recvfrom");
        close(sockfd); 
        exit(-1); 
    }
    printf("I'm server, I recieved \"%s\"\n", s);
    if (sendto(sockfd, "Hello, I'm server!", 19, 0, (struct sockaddr*)client, len) < 0){
        perror("ERROR sendto");
        close(sockfd); 
        exit(-1); 
    }
}

void UDPcli(int sockfd, struct sockaddr_in *server) {
    char s[256];
    int len = sizeof(*server);

    if (sendto(sockfd, "Hello, I'm client!", 19, 0, (struct sockaddr*)server, len) < 0){
        perror("ERROR sendto");
        close(sockfd); 
        exit(-1); 
    }
    if (recvfrom(sockfd, s, sizeof(s), 0, (struct sockaddr*)server, &len) < 0){
        perror("ERROR recvfrom");
        close(sockfd); 
        exit(-1); 
    }
    printf("I'm client, I recieved \"%s\"\n", s);
}

int main(int argc, char const* argv[]) 
{
    char ip[256] = "127.0.0.1"; // DEFAULT
    int PORT1, PORT2;
    int type = SOCK_STREAM;
    int protocol = 0;
    _Bool is_client = 0;
    for(int i = 1; i < argc; ++i) {
        if(strcmp("-h", argv[i]) == 0 || strcmp("--help", argv[i]) == 0) {
            printf("\
                        This is client and server program in one. To use it you should \n\
                    specify PORT which is server port. If You want to execute client use \n\
                    --client with IP. You can also specify -p or --protocol \n\
                    for \"UDP\" or \"TCP\" (default is TCP).\
                    \n\n");
            exit(0);
        }
        if(( strcmp("-p", argv[i]) == 0 || strcmp("--protocol", argv[i]) == 0) && i != argc - 1) {
            if(strcmp("UDP", argv[i + 1]) == 0) {
                type = SOCK_DGRAM;
            }
            else if(strcmp("TCP", argv[i + 1]) != 0) {
                printf("Unknown protocol\n");
                exit(0);
            }
        }
        if(strcmp("--client", argv[i]) == 0 && i != argc - 1) {
            sscanf(argv[i + 1], "%s", ip);
            is_client = 1;
        }
    }
    if(argc < 2) {
        printf("Usage: ./%s PORT\n", argv[0]);
        exit(0);
    }
    sscanf(argv[1], "%d", &PORT1);

    int sockfd; 
   
    sockfd = socket(AF_INET, type, protocol); 
    if (sockfd < 0) {
        perror("ERROR socket");
        exit(-1); 
    }

    struct sockaddr_in server;
    fill_info(&server, PORT1, ip);

    if(is_client) { // client
        if(type == SOCK_STREAM) { // client TCP
            TCPcli(sockfd, &server);
        }
        else if(type == SOCK_DGRAM) { // client UDP
            UDPcli(sockfd, &server); 
        }
        close(sockfd); 
    }
    else { // server
        struct sockaddr_in client;
        int option = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
        if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) != 0) { 
            perror("ERROR bind");
            close(sockfd); 
            exit(-1); 
        }
        if(type == SOCK_STREAM) { // TCP server
            TCPserver(sockfd, &client);
        }
        else if(type == SOCK_DGRAM) { // UDP server
            UDPserver(sockfd, &client);
        }
        close(sockfd);
    }
    
    return 0;
}