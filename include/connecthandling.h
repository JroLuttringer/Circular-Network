#ifndef CONNECTHANDLING_H
#define CONNECTHANDLING_H

#include "constants.h"
#include "structures.h"
#include "check.h"
#include <time.h> /* For the timer in try_connect()*/
#include <string.h> /* For memset */
#include <unistd.h>/* For write*/

/* CONNECT HANDLING :
 * contient les fonctions relatives à la gestion de 
 * connexion des sockets et au maintien des structures
 * utilisées.
 */



/* fonction : init_socket()
 * Créer une socket Stream AF_INET , et vérifie
 * Si aucune erreur ne s'est produite lors de la création
 */
SOCKET init_socket();

/* fonction : init_sockaddr_in
 * param : sockaddr_in*; structure à initialiser
 * param : port ; 
 * Initialise la structure passé en argument à
 * -AF_INET, INADDR_ANY, et le port passé en argument
 */
void init_sockaddr_in(struct sockaddr_in* si,int port);

/*fonction : send_ids
 * sock : socket sur la quelle envoyé les ids
 * id : id du nouveau pair
 * prev : id du predecesseur
 * port : port d'écoute interne du noeud ayant été rejoins
 */
void send_ids(SOCKET sock, int id, int prev,int port);

/* init_data_new_peer
 * Initialise une structure peer_data avec les valeurs passées 
 * en arguments et renvoie la structure 
 */
peer_data init_data_new_peer(int id, struct sockaddr_in si, int port);

/* try_connect
 * Tente de connecter la socket sock_next aux infos peer_data
 * durant TIMEOUT secondes. Si la connection n'a pas pu se faire ,
 * renvoie CO_ERR (définie à -1).
 * Cette fonction a pour but d'empêcher qu'une connexion qui ne 
 * peut pas se faire bloque tout l'anneau.
 */
int try_connect(SOCKET sock_next,peer_data next,int timeout);

#endif /* __CONNECTHANDLING_H__ */
