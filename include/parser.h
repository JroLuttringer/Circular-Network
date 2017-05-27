#ifndef PARSER_H
#define PARSER_H
#include <stdio.h> /* for puts */

/* PARSER
 * contients les fonctions relatives au parse des commandes,
 * une fonction Max, utilisé pour le select, et une fonction dispo_info
 * affichant les arguments qui lui sont passé
 */


// Les #define de ces valeurs sont définies dans constantes.h

int parse(char* cmd);

int max(int a, int b, int c);

void disp_info_pre_co(char* addr, int serv_port, int ext_port, int inport);

void disp_info_init(int serv_port, int inport);

#endif /* __PARSER_H__ */
