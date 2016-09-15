all:
	g++ main.cpp client_example.c server_example.c -o PeerToPeerChat -lpthread

clean:
	rm -f client server PeerToPeerChat