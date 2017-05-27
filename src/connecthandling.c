#include "connecthandling.h"

SOCKET init_socket() {
	SOCKET s = socket(AF_INET,SOCK_STREAM,0);
	CHECK(s != -1);
	return s;
}


void init_sockaddr_in(struct sockaddr_in* si,int port){
	memset((char*)si,'0',sizeof(struct sockaddr_in));
	si->sin_family      = AF_INET;
	si->sin_addr.s_addr = htonl(INADDR_ANY);
	si->sin_port        = htons(port);
}

void send_ids(SOCKET sock, int id, int prev,int port) {
	to_new_peer tnp;
	tnp.own_id  = id;
	tnp.prev_id = prev;
	tnp.port_int = port;
	CHECK( write(sock, &tnp, sizeof(to_new_peer)) != -1);
}



peer_data init_data_new_peer(int id, struct sockaddr_in si, int port) {
	peer_data n;	
	n.id = id;
	n.peer_info = si;
	n.peer_info.sin_port = htons(port);
	return n;
}

int try_connect(SOCKET sock_next,peer_data next,int timeout) {
	puts("################################");
	puts("Trying to connect to :");
	printf("IP: %s\n", inet_ntoa(next.peer_info.sin_addr));
	printf("port is: %d\n", (int) ntohs(next.peer_info.sin_port));
	puts("#################################");
	clock_t start = clock(), diff;
	int msec;
	while((connect (sock_next, (struct sockaddr *)&next.peer_info
	, (socklen_t) sizeof(next.peer_info))) == -1) {
		diff = clock() - start;
		msec = diff * 1000 / CLOCKS_PER_SEC;
		if((msec / 1000) > timeout) /* time in seconds */
			return CO_ERR;
	}	
	return 1;
}
