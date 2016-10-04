#include <iostream>
#include <string>
#include "Peer.h"

using namespace std;

int main(int argc, char ** argv)
{
	cout << "\nHello. Program Starting with host "<< argv[1] 
		<<" and port " << argv[2] << "..";
	Peer * peer = new Peer();
	cout << "\nPeer created..";
	peer->chatWith(argv[1], argv[2]);
	cout << "\nCalled chatWith()...";
	delete peer;
	cout << "\nDeleted peer...";
	return 0;
}