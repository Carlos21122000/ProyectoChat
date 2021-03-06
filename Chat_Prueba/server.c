#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>
#include <json-c/json.h>

#define MAX_CLIENTS 100
#define BUFFER_SZ 8000

static _Atomic unsigned int cli_count = 0;
static int uid = 10;
char *connection_date;

/* Client structure */
typedef struct{
	struct sockaddr_in address;
	int sockfd;
	int uid;
	char name[32];
	char ctime[32];
	char ustatus[32];
} client_t;

client_t *clients[MAX_CLIENTS];

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void str_overwrite_stdout() {
    printf("\r%s", "> ");
    fflush(stdout);
}

void str_trim_lf (char* arr, int length) {
  int i;
  for (i = 0; i < length; i++) { // trim \n
    if (arr[i] == '\n') {
      arr[i] = '\0';
      break;
    }
  }
}

void print_client_addr(struct sockaddr_in addr){
    printf("%d.%d.%d.%d",
        addr.sin_addr.s_addr & 0xff,
        (addr.sin_addr.s_addr & 0xff00) >> 8,
        (addr.sin_addr.s_addr & 0xff0000) >> 16,
        (addr.sin_addr.s_addr & 0xff000000) >> 24);
}

/* Add clients to queue */
void queue_add(client_t *cl){
	pthread_mutex_lock(&clients_mutex);

	for(int i=0; i < MAX_CLIENTS; ++i){
		if(!clients[i]){
			clients[i] = cl;
			break;
		}
	}

	pthread_mutex_unlock(&clients_mutex);
}

/* Remove clients to queue */
void queue_remove(int uid){
	pthread_mutex_lock(&clients_mutex);

	for(int i=0; i < MAX_CLIENTS; ++i){
		if(clients[i]){
			if(clients[i]->uid == uid){
				clients[i] = NULL;
				break;
			}
		}
	}

	pthread_mutex_unlock(&clients_mutex);
}

/* Send message to all clients except sender */
void send_message(char *s, int uid){
	pthread_mutex_lock(&clients_mutex);

	for(int i=0; i<MAX_CLIENTS; ++i){
		if(clients[i]){
			if(clients[i]->uid != uid){
				if(write(clients[i]->sockfd, s, strlen(s)) < 0){
					perror("ERROR: write to descriptor failed");
					break;
				}
			}
		}
	}

	pthread_mutex_unlock(&clients_mutex);
}

/* Handle all communication with the client */
void *handle_client(void *arg){
	char buff_out[BUFFER_SZ];
	char *name;
	char *connection_date;
	char *uinterest;
	char *nstatus;
	int *rcode;
	int leave_flag = 0;
	char *json_instruction;
	char *response;

	cli_count++;
	client_t *cli = (client_t *)arg;
	
	while(1){
		bzero(buff_out, BUFFER_SZ);
		char *opcion;
		rcode = 200;
		if (leave_flag) {
			break;
		}
		int receive = recv(cli->sockfd, buff_out, BUFFER_SZ, 0);
		if (receive > 0){
			json_instruction = buff_out;
			struct json_object *instruction = json_object_new_object();
			instruction = json_tokener_parse(json_instruction);
			
			struct json_object *request;
			json_object_object_get_ex(instruction, "request", &request);
			
			struct json_object *body;
			json_object_object_get_ex(instruction, "body", &body);
			
			opcion = json_object_get_string(request);
			printf("%s\n", opcion);
			printf("\n");
			
			if(strcmp(opcion, "INIT_CONEX") == 0){	
				name = json_object_get_string(json_object_array_get_idx(body, 0));
				strcpy(cli->name, name);
				
				for(int i=0; i<MAX_CLIENTS; ++i){
					if(clients[i]){
						if(strcmp(clients[i]->name, name) == 0 && cli->uid != clients[i]->uid){
							rcode = 101;												
						}
					}
				}
				
				if(rcode == 200){								
					connection_date = json_object_get_string(json_object_array_get_idx(body, 1));
					strcpy(cli->ctime, connection_date);	
				}
				
				struct json_object *instructionJ = json_object_new_object();
				json_object_object_add(instructionJ, "code", json_object_new_int(rcode));
				json_object_object_add(instructionJ, "response", json_object_new_string("INIT_CONEX"));
				response = json_object_to_json_string_ext(instructionJ, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY);
				send(cli->sockfd, response, strlen(response), 0);				
			}
			if(strcmp(opcion, "POST_CHAT") == 0){
				char message_all[8000];
				char separador1[10] = "Mensaje: ";
				char separador2[10] = "Por: ";
				char separador3[10] = "Fecha: ";
				char separador4[10] = "\n";
				strcpy(message_all, json_object_get_string(json_object_array_get_idx(body, 0)));

				printf("%s\n", message_all);
				if(strcmp(message_all, "salir") == 0){
					sprintf(message_all, "%s ha salido del chat\n", cli->name);
					printf("%s\n", message_all);
					send_message(message_all, cli->uid);
				}else{
					bzero(message_all, 8000);
					strcat(message_all, separador1);				
					strcat(message_all, json_object_get_string(json_object_array_get_idx(body, 0)));
					strcat(message_all, separador4);
					
					strcat(message_all, separador2);				
					strcat(message_all, json_object_get_string(json_object_array_get_idx(body, 1)));
					strcat(message_all, separador4);
					
					strcat(message_all, separador3);				
					strcat(message_all, json_object_get_string(json_object_array_get_idx(body, 2)));
					
					send_message(message_all, cli->uid);
					str_trim_lf(message_all, strlen(message_all));
					printf("%s -> %s\n", message_all, cli->name);
					bzero(message_all, 8000);
				}				
			}
			if(strcmp(opcion, "PUT_STATUS") == 0){
				nstatus = json_object_get_string(body);
				strcpy(cli->ustatus, nstatus);
				struct json_object *instructionJ = json_object_new_object();
				json_object_object_add(instructionJ, "response", json_object_new_string("PUT_STATUS"));
				json_object_object_add(instructionJ, "code", json_object_new_int(200));
				
				response = json_object_to_json_string_ext(instructionJ, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY);
				send(cli->sockfd, response, strlen(response), 0);
			}
			if(strcmp(opcion, "GET_USER") == 0){
				rcode = 102;
				uinterest = json_object_get_string(body);
				struct json_object *instructionJ = json_object_new_object();
				if(strcmp(uinterest, "all") == 0){
					rcode = 200;
					struct json_object *body = json_object_new_array();
					for(int i=0; i<MAX_CLIENTS; ++i){
						if(clients[i]){																		
							struct json_object *couple = json_object_new_array();
							json_object_array_add(couple,json_object_new_string(clients[i]->name));
							json_object_array_add(couple,json_object_new_string(clients[i]->ustatus));
							json_object_array_add(body,couple);								
						}
					}															
					json_object_object_add(instructionJ, "code", json_object_new_int(rcode));
					json_object_object_add(instructionJ, "response", json_object_new_string("GET_USER"));
					json_object_object_add(instructionJ,"body", body);
					response = json_object_to_json_string_ext(instructionJ, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY);
					send(cli->sockfd, response, strlen(response), 0);
				}else{
					for(int i=0; i<MAX_CLIENTS; ++i){
						if(clients[i]){
							if(strcmp(clients[i]->name, uinterest) == 0){	
								rcode = 200;												
								struct json_object *body = json_object_new_array();
								json_object_array_add(body,json_object_new_string("127.0.0.1"));
								json_object_array_add(body,json_object_new_string(clients[i]->ustatus));
								json_object_object_add(instructionJ,"body", body);	
							}
						}
					}										
					json_object_object_add(instructionJ, "code", json_object_new_int(rcode));
					json_object_object_add(instructionJ, "response", json_object_new_string("GET_USER"));
					response = json_object_to_json_string_ext(instructionJ, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY);
					send(cli->sockfd, response, strlen(response), 0);
				}
			}
			if(strcmp(opcion, "END_CONEX") == 0){
				leave_flag = 1;
			}
		}
		bzero(buff_out, BUFFER_SZ);
	}

	/* Delete client from queue and yield thread */
	close(cli->sockfd);
	queue_remove(cli->uid);
	free(cli);
	cli_count--;
	pthread_detach(pthread_self());

	return NULL;
}

int main(int argc, char **argv){
	if(argc != 2){
		printf("Usage: %s <port>\n", argv[0]);
		return EXIT_FAILURE;
	}

	char *ip = "172.31.21.201";
	int port = atoi(argv[1]);
	int option = 1;
	int listenfd = 0, connfd = 0;
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	pthread_t tid;

	/* Socket settings */
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(ip);
	serv_addr.sin_port = htons(port);

 	/* Ignore pipe signals */
	signal(SIGPIPE, SIG_IGN);

	if(setsockopt(listenfd, SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option,sizeof(option)) < 0){
		perror("ERROR: setsockopt failed");
		return EXIT_FAILURE;
	}

	/* Bind */
	if(bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("ERROR: Socket binding failed");
		return EXIT_FAILURE;
	}

	/* Listen */
	if (listen(listenfd, 10) < 0) {
		perror("ERROR: Socket listening failed");
		return EXIT_FAILURE;
	}

	printf("=== WELCOME TO THE CHATROOM ===\n");

	while(1){
		socklen_t clilen = sizeof(cli_addr);
		connfd = accept(listenfd, (struct sockaddr*)&cli_addr, &clilen);

		/* Check if max clients is reached */
		if((cli_count + 1) == MAX_CLIENTS){
			printf("Max clients reached. Rejected: ");
			print_client_addr(cli_addr);
			printf(":%d\n", cli_addr.sin_port);
			close(connfd);
			continue;
		}
		
		time_t current_time;
    		time(&current_time);
    		connection_date = ctime(&current_time);

		/* Client settings */
		client_t *cli = (client_t *)malloc(sizeof(client_t));
		cli->address = cli_addr;
		cli->sockfd = connfd;
		cli->uid = uid++;
		strcpy(cli->ustatus, "0");

		/* Add client to the queue and fork thread */
		queue_add(cli);
		pthread_create(&tid, NULL, &handle_client, (void*)cli);

		/* Reduce CPU usage */
		sleep(1);
	}

	return EXIT_SUCCESS;
}
