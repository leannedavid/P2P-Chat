all:
	g++ main.cpp client_example.cpp server_example.cpp -o PeerToPeerChat -lpthread

clean:
	rm -f client server PeerToPeerChat