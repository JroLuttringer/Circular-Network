#ifndef CONSTANTS_H
#define CONSTANTS_H

/* Max Buffer size */
#define BUF_SIZE 512

/* Boolean type */
#define FALSE 0
#define TRUE 1
#define BOOLEAN int

/* For try_connect */
#define TIMEOUT 3

/*SOCKET type */
#define SOCKET int

/* for GET function */
#define FILE_NAME_LENGTH 25

/* To parse commands */
#define JOIN  1
#define PING  2
#define BCAST 3
#define GET 4
#define NOTIFY 5
#define EXIT 6


#define SEG_ERR -1 /* If the creation of a segment failed */
#define END "END_OF_FILE" /* When a file was fully transfered */

/*Commands*/
#define PARSE_ERROR -1 /* if the commande is unknown */
#define CO_ERR -1 /* if try_connect times out */




#endif /* __CONSTANTS_H__ */
