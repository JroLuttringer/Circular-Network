#include "parser.h"
#include "constants.h"


int parse(char* cmd){
	if( cmd [0] != ':')
		return PARSE_ERROR;
	if( cmd[1] == 'J' )
		return JOIN;
	if( cmd[1] == 'P' )
		return PING;
	if ( cmd[1] == 'B')
		return BCAST;
	if(cmd[1] == 'G')
		return GET;
	if(cmd[1] == 'E')
		return EXIT;
		
	puts("CMD NOT FOUND");
	return PARSE_ERROR;	
}


//Max fonction
int max(int a, int b, int c)
{
     int m = a;
     (void)((m < b) && (m = b)); 
     (void)((m < c) && (m = c)); 
     return m;
}

void disp_info_init(int inport, int serv_port){
	puts("###############################");
	printf("Initiating a ring :\n");
	printf("Port within the ring %d\n",inport);
	printf("Listening on port %d\n",serv_port);
	puts("###############################\n");
}


void disp_info_pre_co(char* addr, int serv_port, int ext_port, int inport) {
	puts("################################################");
	printf("- Launching connection on addr %s on port %d \n ",addr,serv_port);
	printf("-My Listening port(ext) will be %d\n",ext_port);
	printf("-My Prev port (int) will be %d \n",inport);
	puts("################################################");
	puts("");
}
