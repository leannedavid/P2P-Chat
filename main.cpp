#include <pthread.h>
 #include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include "server.h"
#include "client.h"
 #define NUM_THREADS     5

 void *listen_routine(void *port){
    std::cout << "\n in listen_routine Going to listen on " << (char*)port;
    server_main((char*)port);
    pthread_exit(NULL);

 }
 void *send_routine(void *port){




    char input[100];
    std::cout << "\n\nEnter a message: ";
    std::cin.getline(input, sizeof(input));
    client_main("localhost", (char *)port, input); 
    std::cout << "\n\nMessage sent!: ";


    pthread_exit(NULL);
  
 }

 int main (int argc, char *argv[])
 {


  /*std::cout << "\nHi\n\t1) Listen on " << argv[1] << "\n\t2) Send on " << argv[2] << "\n\nChoice: ";
  int choice;
  std::cin >> choice;
  if(choice == 1){
    server_main(argv[1]);
  } else {
    while(1){
      char input[100];
      std::cout << "\n\nEnter a message: ";
      std::cin.getline(input, sizeof(input));
      client_main("localhost", argv[2], input); 
      std::cout << "\n\nMessage sent!: ";
    }

  }
  exit(0);*/


    pthread_t listener_thread;
    pthread_t sender_thread;

    std::cout << "\nGoing to listen on " << argv[1] 
      << " and broadcast on " << argv[2] 
      << ", arg[0] was " << argv[0] << "\n";

    int listener_thread_error = pthread_create(&listener_thread, NULL, listen_routine, (void *)argv[1]);
     if(listener_thread_error){

      std::cout << "\nError creating listener: code " << listener_thread_error;

     }

    int sender_thread_error = pthread_create(&sender_thread, NULL, send_routine, (void *)argv[2]);


     if(sender_thread_error){

      std::cout << "\nError creating sender: code " << sender_thread_error;

     }
    /* Last thing that main() should do */
    pthread_exit(NULL);
 }