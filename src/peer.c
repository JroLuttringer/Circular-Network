#include <netdb.h> /* for gethostbyname() */
#include "filehandling.h"
#include "connecthandling.h"
#include "parser.h"
#include "segment.h"

int id_rcv;

int main(int argc,char* argv[]){
	BOOLEAN alone = FALSE;	
	int yes = 1;
	int my_id;		
	if(argc != 5 && argc != 4){
		fprintf(stderr,"Usage : %s port_number_int port_number_ext addr_host port_host\n OU ",argv[0]);
		fprintf(stderr,"Usage : %s port_number_int port_number_ext INIT\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	peer_data next,prev;
	if( !strcmp(argv[3], "INIT")) {
		alone = TRUE;
		create_shared_dir(0);
		my_id = 0;	
		next.id = 0;
		prev.id =0;
	}	
	int prev_port = atoi(argv[1]);
	int external_port = atoi(argv[2]);
	char	buf[BUF_SIZE]; /* data buffer*/
	socklen_t addrlen;
	fd_set read_fds;	/* Select preparation */

	/* INIT SOCKETS */
	SOCKET sock_next = init_socket();
	SOCKET sock_listening = init_socket();
	SOCKET sock_eprev = init_socket();
	SOCKET sock_prev = -1; // initialisation pour éviter les erreurs valgrind
	
	CHECK( (setsockopt(sock_listening, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) != -1));
	CHECK( (setsockopt(sock_eprev, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) != -1));

	/*Init : Exterior */
	struct sockaddr_in listening_on; //to stock new connection
	init_sockaddr_in(&listening_on,external_port);
	CHECK( bind(sock_listening, (struct sockaddr*)&listening_on, 
		 sizeof(listening_on)) != -1 );

	/*Init : Intern */
	struct sockaddr_in new_peer; /* NEW PEER */
	init_sockaddr_in(&new_peer,prev_port);
	CHECK(bind(sock_eprev,(struct sockaddr*)&new_peer,
		 sizeof(new_peer)) != -1);

	/* Get server */
	if(!alone) {
		struct  hostent * he; 
		CHECK((he = gethostbyname(argv[3])) != NULL);
	
	init_sockaddr_in(&next.peer_info,atoi(argv[4]));
	memcpy(&next.peer_info.sin_addr.s_addr,he->h_addr,
			sizeof(next.peer_info.sin_addr.s_addr));
	}	
	CHECK(listen(sock_listening,5) != -1 );
	CHECK(listen(sock_eprev,5) != -1);

	if(!alone) {
		disp_info_pre_co(argv[3],atoi(argv[4]),external_port,prev_port);
		/* try to connect */	
		if(try_connect(sock_next,next,TIMEOUT) == CO_ERR){
			puts("Can't connect to node. Exiting");
			exit(EXIT_FAILURE);
		}
		puts("Connected");		
		int cmd = 0;

		struct timeval t;
		t.tv_sec = 8;
		t.tv_usec = 0;
		fd_set new ;
		FD_ZERO(&new);
		FD_SET(0, &new);
		int rcvd_smth = select(1, &new, NULL, NULL, &t);
		if(rcvd_smth == -1) {
			perror("Select Service");
			close(sock_next);
			exit(EXIT_SUCCESS);
		} else if(rcvd_smth == 0) {
			puts("You were inactive for too long");
			close(sock_next);
			exit(EXIT_SUCCESS);
		} else {
			memset(&buf,'0',BUF_SIZE);
			read(0, &buf, sizeof(segment));
			cmd = parse(buf);
			if(cmd != JOIN){
				puts("You can only Join the peer since you're not in the ring.");
				puts("Please re-run the programe and use the :J command");
				close(sock_next);
				exit(EXIT_SUCCESS);
			}
		}
		segment x;
		create_join_segment(&x);
		write(sock_next,&x,sizeof(segment)); /*sending main node cmd */

		/*Receiving co. info*/
		to_new_peer tnp;
		read(sock_next, &tnp, sizeof(to_new_peer));
		printf("My Id : %d \n",tnp.own_id);
		my_id 	= tnp.own_id;
		prev.id = tnp.prev_id;
		next.peer_info.sin_port = htons(tnp.port_int); /* port from ext to int*/
		create_shared_dir(my_id);
		
		/*Sending listening port*/
		write(sock_next,&prev_port,sizeof(int));		
		puts("Waiting for prev to connect ...");
		addrlen = sizeof(prev.peer_info);
		sock_prev = accept(sock_eprev, (struct sockaddr *)&prev.peer_info
					,&addrlen);
		CHECK(sock_prev > 0);		
		puts("New connection accepted");
		puts("#########################");
	}
	if(alone){
		disp_info_init(prev_port,external_port);
		puts("Waiting for connection ...\n");
	}
		
	while(TRUE) {
		FD_ZERO(&read_fds); /* Init fd_set */
		FD_SET(sock_eprev, &read_fds);
		if(!alone) FD_SET(sock_prev, &read_fds);
		FD_SET(sock_next, &read_fds);
		FD_SET(sock_listening, &read_fds);
		FD_SET(0, &read_fds);
		int m = max(sock_prev,sock_next,sock_listening);
		int activity = select(m+1, &read_fds,NULL,NULL,NULL);
		CHECK(activity > 0);
		if(!activity) continue ;		
		
		if(FD_ISSET(0, &read_fds)) { /* user entered a cmd */
			memset(&buf,0,BUF_SIZE);
			read(0,&buf,BUF_SIZE);
			int cmd = parse(buf); /*cmd rcvd */
			switch(cmd) {				
				case PING:{		/*USR WANTS TO PING */
					if(alone){
						printf("RING : {%d} \n",my_id);
						break;
					}
					segment ping; //HERE
					create_ping_segment(&ping,my_id);
					write(sock_next,&ping,sizeof(segment));
					break;
				}

				case BCAST :{   /*USR WANTS TO BCAST A MESSAGE */
					if(alone){
						printf("You're alone in the Ring\n");
						break;
					}

					segment bcast;	
					if(create_bcast_segment(&bcast,my_id,buf) == SEG_ERR) {
						puts("Error while creating Bcast message");
						break;
					}
					write(sock_next, &bcast,sizeof(segment));
					break;
				}
				case GET : { /* USR WANTS TO GET A FILE */
					if(alone){
						printf("You're alone in the Ring\n");
						break;
					}					
					segment get;
					if(create_get_segment(&get,my_id,external_port, buf) == SEG_ERR){
						puts("Error while create Get segment");
						break;
					}
					write(sock_next,&get,sizeof(segment));
					break;
				}

				case EXIT : { /*USR WANTS TO LEAVE */
					if(!alone){
						segment ex;
						create_exit_segment(&ex,prev.id);
						write(sock_prev, &next, sizeof(peer_data));
						write(sock_next, &ex, sizeof(segment));
					}
					rm_shared_dir(my_id);
					close(sock_prev);
					close(sock_next);
					close(sock_listening);
					return EXIT_SUCCESS;
				}				
				case PARSE_ERROR :
					puts("Command not recognized");
				break;

				default:
				break;
			}/* switch(cmd ) */			
		} else if (!alone && FD_ISSET(sock_prev, &read_fds))	{	/* END IF ISSET 0*/
			segment rcv;
			memset(&rcv,0,sizeof(segment));
			read(sock_prev, &rcv, sizeof(segment));
			switch(rcv.type) {
				case PING :					
					if(rcv.emitter == my_id) 
						printf("PING : {%s}\n",rcv.buffer);
					else {
						sprintf(rcv.buffer,"%s : %d%c",	rcv.buffer,my_id,'\0');
						write(sock_next, &rcv,sizeof(segment));
					}
				break; 

				case BCAST :
					if(rcv.emitter != my_id){
						printf("%d says %s\n",rcv.emitter,rcv.buffer);
						write(sock_next,&rcv,sizeof(segment));						
					} else {
						printf("I said : %s\n",rcv.buffer);
					}
				break; 
				
				case GET:
					if(rcv.emitter == my_id)
						puts("Nobody got this file\n");
					else {
						if(!strlen(rcv.passed_by))
							rcv.emitter_info.peer_info.sin_addr = prev.peer_info.sin_addr;
						
						sprintf(rcv.passed_by,"%s:%d%c",
								rcv.passed_by,my_id,'\0');
						if(!lookup(my_id,rcv.buffer))
							write(sock_next,&rcv,sizeof(segment));
						else{ 							
							SOCKET socket_tmp = init_socket();
							if(try_connect(socket_tmp,rcv.emitter_info,
										TIMEOUT) == CO_ERR)
							{
								puts("Error while connecting");
							}
							create_notify_segment(&rcv);
							write(socket_tmp, &rcv, sizeof(segment));
							send_file(socket_tmp,rcv.buffer, my_id);
							close(socket_tmp);
						} 							
					}											
				break; /* Case GET */
				
				case EXIT: {
					id_rcv = atoi(rcv.buffer);
					if(id_rcv == my_id){
						prev.id = my_id;
						next.id = my_id;
						alone = TRUE;
						close(sock_prev);
						close(sock_next);
						FD_CLR(sock_prev, &read_fds);
						FD_CLR(sock_next, &read_fds);
						sock_next = init_socket();						
						break;
					}
					addrlen = sizeof(prev.peer_info);
					SOCKET socket_tmp = init_socket();
					struct sockaddr_in test;
					addrlen = sizeof (test);
					socket_tmp = accept(sock_eprev, (struct sockaddr*)&test,
						&addrlen);
					sock_prev = socket_tmp;
					prev.id = id_rcv;
				break;
			}

				default:
				break;
			} /* switch(type) */
		}else if(!alone && FD_ISSET(sock_next, &read_fds)) {
			peer_data d; /* getting new succ infos */
			memset(&d,0,sizeof(peer_data));
			read(sock_next, &d, sizeof(peer_data));
			next.id = d.id;
			memset(&next, 0, sizeof(peer_data));
			memcpy(&next.peer_info,&d.peer_info,sizeof(next.peer_info));
			int socket_tmp = init_socket();
			if(try_connect(socket_tmp,next,TIMEOUT) == CO_ERR)
				puts("Couldn't connect to new succ");
			sock_next = socket_tmp;	
		} else if(FD_ISSET(sock_listening, &read_fds)) {
			puts("################################");
			puts("Someone connected");
			struct sockaddr_in tmp ;
			addrlen = sizeof(tmp);
			SOCKET service = accept(sock_listening, (struct sockaddr*)&tmp,
								  &addrlen);
			segment rcv;

			/* Read timeout */
			struct timeval t;
			t.tv_sec = 8;
			t.tv_usec = 0;
			fd_set new ;
			FD_ZERO(&new);
			FD_SET(service, &new);
			int rcvd_smth = select(service + 1, &new, NULL, NULL, &t);
			if(rcvd_smth == -1) {
				perror("Select Service");
				close(service);
				continue;;
			} else if(rcvd_smth == 0) {
				puts("New User didn't enter anything");
				close(service);
				continue;
			} else {
				read(service, &rcv, sizeof(segment));
			}

			switch(rcv.type) {
				case NOTIFY:
					printf("Beginning to receive file %s",rcv.buffer);
					rcv_file(service,rcv.buffer,my_id);
					puts("File Transfered");
					printf("File Found via : %s\n",rcv.passed_by);
					puts("######################################\n");
					close(service);
				break;
				case JOIN : {
					srand(time(NULL));
					int id_new_peer = rand() % 255; 
					send_ids(service,id_new_peer,prev.id,prev_port);						
					int new_port;
					read(service, &new_port, sizeof(int));
					peer_data new_peer = init_data_new_peer(id_new_peer,tmp,new_port);
					if(!alone){ 
						write(sock_prev, &new_peer, sizeof(peer_data));			
					} else {
						next.id = id_new_peer;
						next.peer_info = new_peer.peer_info;			
						if(try_connect(sock_next, next, TIMEOUT) == -1) {
							puts("Can't connect to new peer");
							break;
						} else {
							puts("	Connected to new peer");	
						}
					}					
					prev.peer_info = new_peer.peer_info;
					prev.id = id_new_peer;
					sock_prev = service;										
					alone = FALSE;
					puts("Someone joined the ring");
					puts("#################################################\n");						
					break;
				} // JOIN
				default:
					puts("New peer entered a command unknow. Closing connection");
					close(service);
				break;
			} /* switch rcv.type */			
		}
	}/* while(TRUE) */
	
	return 0;
}
