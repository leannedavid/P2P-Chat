
#ifndef PEER_H
#define PEER_H

#include <string>
#include <vector>
#include <iterator>
#include <map>


class Peer
{
private:
	std::string ip_address;
	std::string port;
	std::vector<Peer> * peers;
	static std::map<std::string, Peer> addressesToPeersMap;
public:
	Peer(std::string,std::string);
	void addPeer(Peer);
	void sendMessageToPeer(std::string,Peer);
	void sendMessageToAll(std::string);
	~Peer();

};

#endif