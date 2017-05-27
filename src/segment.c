#include "segment.h"
#include "connecthandling.h" /* for init sockaddr_in */

int create_ping_segment(segment* s, int id) {
	memset(s,0,sizeof(segment));
	s->emitter = id;
	s->type = PING;
	sprintf(s->buffer,"%d%c",id,'\0');	
	return 1;
}


int create_exit_segment(segment *s, int id) {
	memset(s,0,sizeof(segment));
	s->type = EXIT;
	sprintf(s->buffer,"%d%c",id,'\0');
	return 1;
}

int create_bcast_segment(segment* s,int id,char* buffer) {
	memset(s,0,sizeof(segment));
	char* message = strtok(buffer," ");
	message = strtok(NULL,"\n");
	if(message == NULL || strlen(message) > BUF_SIZE-1)
		return SEG_ERR;
	s->emitter = id;
	s->type = BCAST;
	sprintf(s->buffer,"%s%c",message,'\0');
	return 1;
}

int create_get_segment(segment* s, int id,int port, char* buffer){
	memset(s,0,sizeof(segment));
	s->type = GET;
	s->passed_by[0] = '\0';
	char* file_name = strtok(buffer," ");
	file_name = strtok(NULL,"\n");
	if(file_name == NULL || strlen(file_name) > BUF_SIZE-1)
		return SEG_ERR;
	s->emitter = id;
	init_sockaddr_in(&s->emitter_info.peer_info, port);
	sprintf(s->buffer,"%s%c",file_name,'\0');
	return 1;
}

int create_join_segment(segment *s) {
	memset(s,0,sizeof(segment));
	s->type = JOIN;
	return 1;
}

int create_notify_segment(segment *s) {
	s->type = NOTIFY; 
	return 1;
}
