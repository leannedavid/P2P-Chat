#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <errno.h>


#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <signal.h>


#include "Connection.h"


using namespace std;

#define NUM_THREADS     5



#define BACKLOG 10     // how many pending connections queue will hold

void sigchld_handler_main(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr_server(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int server_main(char * port)
{
	//char * port = (char*)portVd;W
	string ok;
	/*cout << "\nin server_main(). OK?";
	cin >> ok;*/
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
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
        return 1;
    }
	cout << "\ndone with getaddrinfo(). OK?";
	//cin >> ok;
	int index = 0;
    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {

		/*cout << "\ntraversing element" << index << ". OK?";
		cin >> ok;
		index++;*/

        //std::cout << "\n*****\nlooking at p->ai_canonname " << p->ai_canonname << "\n*****\n";


        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
        else 
        {

        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler_main; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    //printf("\n** server: waiting for connections **\n");

    while(1) {  // main accept() loop
        sin_size = sizeof peer_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&peer_addr, &sin_size);
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
            get_in_addr_server((struct sockaddr *)&peer_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);

        /*if (!fork()) { // this is the child process
            close(sockfd); // child doesn't need the listener
            if (send(new_fd, "Hello, world!", 13, 0) == -1)
                perror("send");
            close(new_fd);
            exit(0);
        }*/
        close(new_fd);  // parent doesn't need this
    }

    return 0;
}


struct Peer 
{
    char * ip;
    char * port;
};

void *listen_routine(void *port){
    std::cout << "\n In listen_routine\nGoing to listen on " << (char*)port;
    server_main((char*)port);
    pthread_exit(NULL);
}

/*void *send_routine(void *peer){
    char input[100];
    struct Peer * myPeer = (struct Peer *)peer;
    std::cout << "\n\nEnter a message!!!!!: ";
    //std::cin.ignore();
    std::cin.getline(input, sizeof(input));
    client_main(myPeer->ip, myPeer->port, input);
    std::cout << "\n\nMessage sent????: " << "\"" << input << "\"";
    
    
    pthread_exit(NULL);
    
}*/


void display_help(){
    std::cout << "\n**************************** HELP MENU ****************************\n"
    << " :: What would you like to do?\n\n"
    << "- myip:   Display my IP address\n"
    << "- myport: Display port on which this process is listening for\n"
    << "          incoming connections.\n"
    << "- connect <destination> <port no>: Establish a new TCP connection\n"
    << "          to the destination IP address at the specified port #.\n"
    << "- list:   Display a numbered list of all the connections this\n"
    << "          process is part of.\n"
    << "- terminate <connection id>: Terminate the connection listed under\n"
    << "          specified number under list.\n"
    << "- send <connection id> <message>: Send a message to a specified\n"
    << "          connection.\n"
    << "- exit:   Exit the program.\n"
    << "*******************************************************************\n\n\n";
}


int main(int argc, char ** argv)
{
	/*cout << "\nHello. Program Starting with host "<< argv[1] 
		<<" and port " << argv[2] << "..";
	Peer * peer = new Peer();
	cout << "\nPeer created..";
	peer->chatWith(argv[1], argv[2]);
	cout << "\nCalled chatWith()...";
	delete peer;
	cout << "\nDeleted peer...";*/




	vector<Connection*> * myPeers = new vector<Connection*>();
 
    char input[100];
    std::string choice;
    //std::cin >> choice;
    bool exit = true;
    
    string hostToConnect, portToConnect;

    char * myPort = argv[1];

    pthread_t listener_thread;
    int listener_thread_error =
    pthread_create(&listener_thread, NULL, listen_routine, (void *)argv[1]);
    
    
    if(listener_thread_error){
        std::cout << "\nError creating listener: code " << listener_thread_error;
    }



    do{
        
        /*if(choice == "1"){
            server_main(argv[1]);
            
            std::cout << ">> ";
            std::cin >> choice;
        }

        else */
        if(choice == "help" || choice == "HELP"){
            display_help();
        }
        
        else if (choice == "myip" || choice == "MYIP"){


			//char * port = (char*)portVd;W
			string ok;
			/*cout << "\nin server_main(). OK?";
			cin >> ok;*/
		    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
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

		    char * cur;

		    if ((rv = getaddrinfo(NULL, myPort, &hints, &servinfo)) != 0) {
		        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		        return 1;
		    }
		    else 
		    {

			    inet_ntop(servinfo->ai_addr->sa_family,
	            get_in_addr_server((struct sockaddr *)&(*(servinfo->ai_addr))),
	            cur, sizeof cur);

		    }

		    freeaddrinfo(servinfo);



            std::cout << "\nThe IP address is: " << cur << "\n\n";
        }
        
        else if (choice == "myport" || choice == "MYPORT"){
            std::cout << "\nThe port no. is: " << argv[1] << "\n\n";
        }
        
        
        else if (choice == "connect" || choice == "CONNECT"){
            std::cin.ignore();
            //std::cin.getline(input, sizeof(input));
            cin >> hostToConnect >> portToConnect;
            
            std::cout << "\n\nConnecting with " << hostToConnect << ":"<< portToConnect << " ......\n\n";



            Connection * conn = new Connection(hostToConnect, portToConnect);
            myPeers->push_back(conn);
        }
        
        else if (choice == "list" || choice == "LIST"){
            std::cout << "\n\nLISTING CONNECTIONS:\n\n";
            string out = "";
            for(int i = 0; i < myPeers->size(); i++)
            {

            	cout << i << ") " << " " << (*myPeers)[i]->host << ":" << (*myPeers)[i]->port << "\n";

	
            }

        }

        else if (choice == "terminate" || choice == "TERMINATE"){
            std::cin.ignore();
            //std::cin.getline(input, sizeof(input));
            int ind;
            cin >> ind;

            
            std::cout << "\n\nTerminating with " << ind << " ......\n\n";

            Connection * connToDelete = (*myPeers)[ind];
            myPeers->erase(myPeers->begin() + ind);
            delete connToDelete;

        }

        
        else if (choice == "send") {
            std::cin.ignore();
            //std::cout << "\n\nEnter a message: ";

            int idToSend;
            cin >> idToSend;
            std::cin.getline(input, sizeof(input));
            //client_main("localhost", argv[2], input);
            Connection * connToSend = (*myPeers)[idToSend];
            std::cout << "Message that will be sent: " << "\"" << input << "\"\n\n";
            connToSend->sendMessage(input);

        }
        
        if (choice == "exit" || choice == "EXIT"){

            for(int i = 0; i < myPeers->size(); i++)
            {

	            Connection * connToDelete = (*myPeers)[i];
	            myPeers->erase(myPeers->begin() + i);
	            delete connToDelete;

            }
            exit = true;
            break;
        }
        
        
        std::cout << ">> ";
        std::cin >> choice;
        if (choice != "send" || choice != "connect" || choice != "terminate"){
           // std::cout << "CHOICE: " << choice;
        }
        
    } while (choice != "exit");


    std::cout << "\nHi, type \"help\" for assistance:\n\t1) SERVER - Listening on " << argv[1];
              //<< "\n\tsend <message>: CLIENT - Send on " << argv[2] << "\n\n>> ";
   



    
    std::cout << "\n+--------------------------------------------------+\n"
                << "|                 SIGNING OUT....                  |\n"
                << "+--------------------------------------------------+\n";


    pthread_exit(NULL);

    




	return 0;
}

