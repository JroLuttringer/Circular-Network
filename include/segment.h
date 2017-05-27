#ifndef __SEGMENT_H__
#define __SEGMENT_H__

#include "structures.h"
#include "string.h" /* For memset */

/* SEGMENT
	Contient toutes les fonctions relatives à
	la création de segments (unité utilisée pour communiquer)
*/

/*Créer un segment PING ayant pour émetteur le pair ID */
int create_ping_segment(segment* s, int id);

/*Créer un segment EXIT ayant pour émetteur le pair ID */
int create_exit_segment(segment *s, int id);

/*Créer un segment BROADCAST ayant pour émetteur le pair ID 
  contenant le message buffer*/
int create_bcast_segment(segment* s,int id,char* buffer);

/* Créer un segment GET ayant pour émetteur le pair ID,
	joignable sur le port P, et cherchant le fichier buffer
	*/
int create_get_segment(segment* s, int id,int port, char* buffer);

/* Créer un segment Join */
int create_join_segment(segment *s);

/* créer un segment notify
	Ce dernier ré utilise le segment GET envoyé */
int create_notify_segment(segment *s);









#endif /* __SEGMENT_H__ */
