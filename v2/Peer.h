#ifndef PEER_H
#define PEER_H

#include "Connection.h"

using namespace std;

class Peer
{
private:
	Connection * connection;
	pthread_t * listenerThread;
public:
	Peer();
	void chatWith(string,string);
	~Peer();
};

#endif