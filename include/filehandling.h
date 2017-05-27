#ifndef __FILEHANDLING_H__
#define __FILEHANDLING_H__

#include "constants.h"
#include <dirent.h>
#include "check.h"
#include <string.h>
#include <fcntl.h>
#include <unistd.h> /* For read and write*/
#include <sys/stat.h> /* For stat  and mkdir */


/* FILEHANDLING
	Contient les fonctions relatives à la gestion de dossiers
	et fichiers, utilisées dans la commande GET
*/



/* LOOKUP
	Cette fonction va chercher dans le dossier appartenant au 
	pair ID le fichier appelé file_name.
	Renvoie TRUE (1) si le fichier est trouvé, FALSE sinon
*/
BOOLEAN lookup(int id,const char* file_name);


/* SEND_FILE
	Cette fonction transmet le fichier file du pair id au 
	pair connecté sur la socket receiver.
	Quand le fichier a été completement transféré, la fonction
	envoie END_OF_FILE (constante définié dans constantes)
*/
int send_file(SOCKET receiver, const char* file, int id);

/* RCV_FILE
	Cette fonction receptionne dans le dossier du pair ID le fichier
	file étant transmis par le pair connecté sur la socket sender.
	Quand la fonction reçoit END_OF_FILE, elle se termine.
*/
int rcv_file(SOCKET sender, const char* file,int id);

/* RM_SHARED_DIR
	Cette fonction supprime le fichier partagé du pair ID. Elle est
	appelée à deux occasion : lorsque le pair EXIT, ou lors d'un CTRL+C
*/
void rm_shared_dir(int id);

/* fonction récursive utilisée par rm_shared_dir pour supprimer un répertoire
non vide */
int rm_shared_dir2(char* path);	


/* CREATE_SHARED_DIR
	Cette fonction crée un fichier partagé pour le pair ID dans lequel
	seront stockés les fichiers "visibles" par tous les membres de l'anneau 
	(i.e un get pourra être opéré sur ce fichier)
*/
void create_shared_dir(int id);

#endif /* __ FILEHANDLING_H__ */
