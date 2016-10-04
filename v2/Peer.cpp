#include "Peer.h"
#include <iostream>
#include <pthread.h>
using namespace std;

Peer::Peer()
{
	cout << "\nnew user created.";
}

void * listenFunction(void * connectionPtr){
    char choice;
    cout << "\nI am in the listener function. Ok? (Y/n) ";
    cin >> choice;
    Connection * connection = (Connection*)connectionPtr;
    cout << "\nCast void ptr back to Connection*. Ok? (Y/n) ";
    cin >> choice;
    connection->listenForMessages();
    cout << "\nDone listening for messages. Ok? (Y/n) ";
    pthread_exit(NULL);
 }

void Peer::chatWith(string host,string port)
{
    cout << "\nIn chatWith(), i have host " << host << " and port " << port;
	this->connection = new Connection(host,port);
    cout << "\nConnection instantiated!";

    Connection * connectionPtr = this->connection;
    cout << "\nAssigned this->connection to a temp! Ok (Y/n)?";
    char choice;
    cin >> choice;

    pthread_t listenThread;

    int err = pthread_create(&listenThread, NULL, listenFunction, (void*)this->connection);
    if(err)
    {
    	cout << "\nError creating listener thread: " << err;
    }
    else
    {
        cout << "\nlistener thread created!";
    }
    this->listenerThread = &listenThread;
    while(1)
    {
    	cout << "\nEnter message: ";
    	string message;
    	cin >> message;
    	this->connection->sendMessage(message);
    }
    pthread_exit(NULL);

}

Peer::~Peer()
{
	delete this->connection;
}