#include "Peer.h"

Peer::Peer(std::string ip_address,std::string port){
	this->ip_address =ip_address;
	this->port = port;
	this->peers = new std::vector<Peer>();
}
void Peer::addPeer(Peer peer){
	this->peers->push_back(peer);
}
void Peer::sendMessageToPeer(std::string message,Peer peer){

}
void Peer::sendMessageToAll(std::string message){
	std::vector<Peer>::iterator it;
	for(it = this->peers->begin(); it != this->peers->end(); ++it){
		this->sendMessageToPeer(message, *it);
	}
}

Peer::~Peer(){
	delete this->peers;
}