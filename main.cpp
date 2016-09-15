#include <pthread.h>
 #include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include "server.h"
#include "client.h"
 #define NUM_THREADS     5
#define LISTENER_PORT 5555
#define SENDER_PORT 8888

 void *listen_routine(void *port){
    long tid;
    tid = (long)port;
    printf("Hello World! It's me, listener thread #%ld!\n", tid);
    server_main();
    pthread_exit(NULL);

 }
 void *send_routine(void *port){
    long tid;
    tid = (long)port;
    printf("Hello World! It's me, sender thread #%ld!\n", tid);
    client_main("localhost");
    pthread_exit(NULL);
  
 }

 int main (int argc, char *argv[])
 {
    pthread_t listener_thread;
    pthread_t sender_thread;

    int listener_thread_error = pthread_create(&listener_thread, NULL, listen_routine, (void *)LISTENER_PORT);
     if(listener_thread_error){

      std::cout << "\nError creating listener: code " << listener_thread_error;

     }

    int sender_thread_error = pthread_create(&sender_thread, NULL, send_routine, (void *)SENDER_PORT);


     if(sender_thread_error){

      std::cout << "\nError creating sender: code " << sender_thread_error;

     }
    /* Last thing that main() should do */
    pthread_exit(NULL);
 }