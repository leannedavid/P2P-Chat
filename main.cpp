#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include "server.h"
#include "client.h"
#include "Peer.h"
#include <arpa/inet.h>

 void *listen_routine(void *port){
    std::cout << "\n In listen_routine\nGoing to listen on " << (char*)port;
    server_main((char*)port);
    pthread_exit(NULL);
 }

 void *send_routine(void *port){
    char input[100];
    std::cout << "\n\nEnter a message: ";
    //std::cin.ignore();
    std::cin.getline(input, sizeof(input));
    client_main("localhost", (char *)port, input); 
    std::cout << "\n\nMessage sent: " << "\"" << input << "\"";


    pthread_exit(NULL);
  
 }


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


 int main (int argc, char *argv[]){
     
     std::cout << "\nHi, type \"help\" for assistance:\n\t1) Listen on " << argv[1] << "\n\t2) Send on " << argv[2] << "\n\nChoice: ";
     std::string choice;
     std::cin >> choice;
     

     if(choice == "1"){
         server_main(argv[1]);
     }
  
     else if (choice == "2") {
         while(1){
             char input[100];
             std::cout << "\n\nEnter a message: ";
             std::cin.getline(input, sizeof(input));
             client_main("localhost", argv[2], input);
             std::cout << "\n\nMessage sent: " << "\"" << input << "\"";

         }
    }
     
    else if(choice == "help" || choice == "HELP"){
         display_help();
          
    }
  
    //exit(0);


    pthread_t listener_thread;
    pthread_t sender_thread;

    std::cout << "\nGoing to listen on " << argv[1] 
              << " and broadcast on "    << argv[2]
              << ",\n\targ[0] was \""      << argv[0] << "\"\n";

    int listener_thread_error =
        pthread_create(&listener_thread, NULL, listen_routine, (void *)argv[1]);
     
     
    if(listener_thread_error){
      std::cout << "\nError creating listener: code " << listener_thread_error;
     }

    int sender_thread_error =
        pthread_create(&sender_thread, NULL, send_routine, (void *)argv[2]);


    if(sender_thread_error){
        std::cout << "\nError creating sender: code " << sender_thread_error;
    }
     
    /* Last thing that main() should do */
    pthread_exit(NULL);
 }