#ifndef CONNECTION_H
#define CONNECTION_H
#include <string>

using namespace std;

class Connection
{
private:
	int senderSocketFileDescriptor;
	int listenerSocketFileDescriptor;
	const char * host;
	const char * port;
	void connectSenderSocket();
	void connectListenerSocket();
	void * get_in_addr(struct sockaddr *);
public:
	Connection(string,string);
	void initialize();
	void disconnect();
	void sendMessage(string);
	//void sigchld_handler(int);
	void listenForMessages();
	~Connection();

};

#endif

