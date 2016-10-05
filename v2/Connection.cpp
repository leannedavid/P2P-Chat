#include "Connection.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <iostream>


#define BACKLOG 10

using namespace std;




Connection::Connection(string hostString, string portString)
{	
    this->portString = portString;
    this->host = hostString.c_str();
	this->port = portString.c_str();
    cout << "\ncreating connection with host " << this->host <<" and port" << this->port;
    this->initialize();

}

void * Connection::get_in_addr(struct sockaddr * sa)
{

    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);

}
void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

        while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

void Connection::initialize()
{


    //this->connectListenerSocket();
    this->connectSenderSocket();
    string myPort = "";
    myPort += "+";
    myPort += this->portString;
    cout << "\nin Connection::initialize(): sending " << myPort;
    this->sendMessage(myPort);



}
void Connection::disconnect()
{
    close(this->senderSocketFileDescriptor);
    close(this->listenerSocketFileDescriptor);

}
void Connection::sendMessage(string message)
{
    const char * msg = message.c_str();
    if (send(this->senderSocketFileDescriptor, msg, strlen(msg), 0) == -1)
        perror("send");
}
void Connection::listenForMessages()
{
    cout << "\nI am in connection's listenForMessages. Ok? (Y/n) ";
	int new_fd;
    socklen_t sin_size;
    struct sockaddr_storage peer_addr; 
    char s[INET6_ADDRSTRLEN];

    while(1) {  // main accept() loop
        sin_size = sizeof peer_addr;
        new_fd = accept(this->listenerSocketFileDescriptor, (struct sockaddr *)&peer_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }
        int numbytes;

        char buf[100];
        if ((numbytes = recv(new_fd, buf, 99, 0)) == -1) {
            perror("recv");
            exit(1);
        }

        buf[numbytes] = '\0';

        printf("server received message: '%s'\n",buf);

        inet_ntop(peer_addr.ss_family,
            this->get_in_addr((struct sockaddr *)&peer_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);

        /*if (!fork()) { // this is the child process
            close(this->listenerSocketFileDescriptor); // child doesn't need the listener
            if (send(new_fd, "Hello, world!", 13, 0) == -1)
                perror("send");
            close(new_fd);
            exit(0);
        }*/
        close(new_fd);  // parent doesn't need this
    }
}

void Connection::connectSenderSocket()
{


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
        return;
    } else {
        std::cout << "getaddrinfo() good!\n";
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((this->senderSocketFileDescriptor = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        } else {
            std::cout << "socket() good!\n";
        }

        if (connect(this->senderSocketFileDescriptor, p->ai_addr, p->ai_addrlen) == -1) {
            close(this->senderSocketFileDescriptor);
            perror("client: connect");
            continue;
        } else {
            std::cout << "connect() good!\n";
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return;
    } else {
        std::cout << "client connected! p is not null...\n";
    }

   /* inet_ntop(p->ai_family, get_in_addr_client((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);*/

    freeaddrinfo(servinfo); // all done with this structure

}
void Connection::connectListenerSocket()
{

    int new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage peer_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return;
    }


    std::cout << "\nrv is " << rv;


    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {

        //std::cout << "\n*****\nlooking at p->ai_canonname " << p->ai_canonname << "\n*****\n";


        if ((this->listenerSocketFileDescriptor = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
        else 
        {
            std::cout << "\n*****\nSocket ctor successful\n*****\n";

        }

        if (setsockopt(this->listenerSocketFileDescriptor, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        else
        {
            std::cout << "\n*****\nSocket setsockopt successful\n*****\n";

        }

        if (bind(this->listenerSocketFileDescriptor, p->ai_addr, p->ai_addrlen) == -1) {
            close(this->listenerSocketFileDescriptor);
            perror("server: bind");
            continue;
        }
        else
        {

            std::cout << "\n*****\nSocket bind successful\n*****\n";
        }

        break;
    }
    std::cout << "\n*****\nSocket successful\n*****\n";

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(this->listenerSocketFileDescriptor, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

}



Connection::~Connection()
{

}