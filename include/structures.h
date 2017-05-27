#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <arpa/inet.h> 
#include "constants.h"

/* STRUCTURES
	Contient les structures utilisés lors de communication entre
	pairs
	*/



/*Structure sent to a new peer immediatly after his :JOIN demand */
struct to_new_peer{
	int own_id;
	int prev_id;
	int port_int;
};
typedef struct to_new_peer to_new_peer;


/*used to stock info relative to peer*/
struct peer_data{
	struct sockaddr_in peer_info;
	int id;
};
typedef struct peer_data peer_data;

/* Message structure */
struct segment{
	int emitter; //ID du pair ayant émis le segment
	int type;	// type du message porté par le segment
	char passed_by[BUF_SIZE]; // par qui le segment est passé (utilisé uniquement dans GET)
	char buffer[BUF_SIZE]; // données transmises
	peer_data emitter_info; // Où joindre l'emetteur (utilisé uniquement dans GET)
};
typedef struct segment segment;




#endif /* __STRUCTURES_H */
