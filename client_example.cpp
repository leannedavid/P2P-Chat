/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>

#include <arpa/inet.h>


#define MAXDATASIZE 100 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr_client(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int client_main(char * host, char * port, char * message)
{
    int sockfd;  
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    //std::cout << "in client_main with message " << message << " and port " << port << "\n";

    if (strlen(host) < 1) {
        fprintf(stderr,"Error! usage: client hostname\n");
        exit(1);
    } else {
        //std::cout <<"host name good, you sent " << host << "\n";
    }

    if(memset(&hints, 0, sizeof hints) > 0){
        //std::cout << "memset returnes a positive value\n";
    } else {
        //std::cout << "memset returnes a negative value\n";
    }
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(host, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    } else {
        //std::cout << "getaddrinfo() good!\n";
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        } else {
            //std::cout << "socket() good!\n";
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        } else {
            //std::cout << "connect() good!\n";
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    } else {
        //std::cout << "client connected! p is not null...\n";
    }

    /*inet_ntop(p->ai_family, get_in_addr_client((struct sockaddr *)p->ai_addr),
            s, sizeof s);*/
    //printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure


    if (send(sockfd, message, strlen(message), 0) == -1)
        perror("send");


    close(sockfd);

    return 0;
}