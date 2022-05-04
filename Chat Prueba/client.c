#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <json-c/json.h>
#include <netdb.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>

#define BUFFER_SZ 8000

char *name;
char *connection_date;
char *message_date;
char *instruction;
char *json_instruction;
char buff_out[BUFFER_SZ];
int socketfd = 0;
char estado[10];
volatile sig_atomic_t flag = 0;


void str_overwrite_stdout(){
	printf("\r%s", "> ");
	fflush(stdout);
}

void str_trim_lf(char* arr, int length){
	for(int i = 0; i < length; i++){
		if(arr[i] == '\n'){
			arr[i] = '\0';
			break;
		}		
	}
}

void catch_ctrl_c_and_exit(){
	flag = 1;
}

void recv_msg_handler(){
	char message[200] = {};
	while(1){
		int receive = recv(socketfd, message, 200, 0);
		
		if(receive > 0){
			printf("%s ", message);
			str_overwrite_stdout();
		}else if(receive == 0){
			break;
		}
		bzero(message, 200);
	}
}

void send_msg_handler(){
	char buffer[200] = {};
	char message[8000] = {};
	
	while(1){
		str_overwrite_stdout();
		fgets(buffer, 200, stdin);
		str_trim_lf(buffer, 200);
		struct json_object *instructionJ = json_object_new_object();
		
		time_t current_time;
	    	time(&current_time);
	    	message_date = ctime(&current_time);
		
		if(strcmp(buffer, "salir") == 0){
			break;
		} else {		    	
			json_object_object_add(instructionJ, "request", json_object_new_string("POST_CHAT"));
			struct json_object *body = json_object_new_array();
			json_object_array_add(body,json_object_new_string(message));
			json_object_array_add(body,json_object_new_string(name));
			json_object_array_add(body,json_object_new_string(message_date));
			json_object_array_add(body,json_object_new_string("all"));
			json_object_object_add(instructionJ,"body", body);
			
			instruction = json_object_to_json_string_ext(instructionJ, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY);
			
			sprintf(message, "%s: %s\n", name, buffer);
			send(socketfd, instruction, strlen(instruction), 0);
		}
	bzero(buffer, 200);
	bzero(message, 8000);
	json_object_put(instructionJ);
	}
	catch_ctrl_c_and_exit(2);
}

int main(int argc, char *argv[])
{
	int opcion;
	int repetir = 1;
	int result;
	int *rcode;
	
	char *StatusFromIU;
	
	char *host;
	
	char username2chat[200];
	
	char statusc[32];
	int listenfd = 0, connfd = 0;
    	struct sockaddr_in serverAddr;
    	struct sockaddr_in cliAddr;
   	long port;
   	pthread_t tid;
   	
   	time_t current_time;
    	time(&current_time);
    	connection_date = ctime(&current_time);
	
	name = argv[1];
	host = argv[2];
	port = atoi(argv[3]);
	
	if(argc != 4)
	{	
		fprintf(stderr, "./client [username] [host] [port]\n");
		exit(1);
	}
	
	if(host == NULL)
	{
		fprintf(stderr, "Couldn't get host name\n");
		exit(1);
	}
	
	signal(SIGINT, catch_ctrl_c_and_exit);
	str_trim_lf(name, strlen(name));
	
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(host);
	serverAddr.sin_port = htons(port);
	
	socketfd = socket(AF_INET, SOCK_STREAM, 0);
				
	int err = connect(socketfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (err == -1){
		printf("ERROR: connect\n");
		return EXIT_FAILURE;
	}		
	
	struct json_object *init_conection = json_object_new_object();
	json_object_object_add(init_conection, "request", json_object_new_string("INIT_CONEX"));
	struct json_object *body = json_object_new_array();
	json_object_array_add(body,json_object_new_string(name));
	json_object_array_add(body,json_object_new_string(connection_date));
	json_object_object_add(init_conection,"body", body);
	
	instruction = json_object_to_json_string_ext(init_conection, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY);
	
	send(socketfd, instruction, strlen(instruction), 0);
	json_object_put(init_conection);
	
	
	int receive = recv(socketfd, buff_out, BUFFER_SZ, 0);		
	if(receive > 0){
		json_instruction = buff_out;
		struct json_object *response= json_object_new_object();
		response = json_tokener_parse(json_instruction);
		
		struct json_object *code;
		json_object_object_get_ex(response, "code", &code);
		rcode = json_object_get_int(code);
		
		if(rcode == 101){
			struct json_object *instructionJ = json_object_new_object();
			printf("\nEse nombre ya esta en uso. Feliz dia...\n");
			
			json_object_object_add(instructionJ, "request", json_object_new_string("END_CONEX"));				
			instruction = json_object_to_json_string_ext(instructionJ, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY);
			
			send(socketfd, instruction, strlen(instruction), 0);
			close(socketfd); 
			return 0;
		}else{
			printf("\nHa ingresado al chat. \n");
		}
	}
	bzero(buff_out, BUFFER_SZ);
		
	while (repetir == 1) {
		struct json_object *instructionJ = json_object_new_object();
		// Texto del menú que se verá cada vez
		printf("\n\nMenu de Opciones");
		printf("\n1. Chat con todos lo usuarios");
		printf("\n2. Enviar y recibir mensajes directos, privados, aparte del chat general");
		printf("\n3. Cambiar de status");
		printf("\n4. Listar los usuarios conectados al sistema de chat");
		printf("\n5. Desplegar información de un usuario en particular");
		printf("\n6. Ayuda");
		printf("\n7. Salir");

		printf("\nIngrese una opcion: ");
		scanf("%d", &opcion);
		

		switch (opcion) {
			case 1:		
				json_object_object_add(instructionJ, "request", json_object_new_string("GET_CHAT"));
				json_object_object_add(instructionJ, "body", json_object_new_string("all"));
				
				instruction = json_object_to_json_string_ext(instructionJ, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY);
				
				send(socketfd, instruction, strlen(instruction), 0);
				json_object_put(instructionJ);
				
				pthread_t send_msg_thread;
				if(pthread_create(&send_msg_thread, NULL, (void *) send_msg_handler, NULL) != 0){
					printf("ERROR: pthread\n");
					return EXIT_FAILURE;
				}

				pthread_t recv_msg_thread;
				if(pthread_create(&recv_msg_thread, NULL, (void *) recv_msg_handler, NULL) != 0){
					printf("ERROR: pthread\n");
					return EXIT_FAILURE;
				}
				
				while(1){
					if(flag){
						printf("\nBye\n");
						break;
					}
				}            
				break;
			case 2:             
				printf("Nombre del usuario para entablar conversacion: ");
				scanf("%s", &username2chat);
				json_object_object_add(instructionJ, "request", json_object_new_string("GET_CHAT"));
				json_object_object_add(instructionJ, "body", json_object_new_string(username2chat));
				
				instruction = json_object_to_json_string_ext(init_conection, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY);
				
				send(socketfd, instruction, strlen(instruction), 0);
				json_object_put(instructionJ);
				char username2chat[200];
				break;

			case 3:   
				// Lista de instrucciones de la opción 3   
				printf("\nOpciones de status");
				printf("\n0. OCUPADO");
				printf("\n1. ACTIVO");
				printf("\n2. INACTIVO");  
				printf("\nIngrese una opcion: ");           
            			scanf("%s", &estado);
            		
            			json_object_object_add(instructionJ, "request", json_object_new_string("PUT_STATUS"));
				json_object_object_add(instructionJ, "body", json_object_new_string(estado));
				
				instruction = json_object_to_json_string_ext(instructionJ, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY);
				
				send(socketfd, instruction, strlen(instruction), 0);
				json_object_put(instructionJ);
				
				receive = recv(socketfd, buff_out, BUFFER_SZ, 0);		
				if(receive > 0){
					printf("Cambio de estado realizado");
				}
				bzero(buff_out, BUFFER_SZ);
            			
				break;
			case 4:
				json_object_object_add(instructionJ, "request", json_object_new_string("GET_USER"));
				json_object_object_add(instructionJ, "body", json_object_new_string("all"));
				
				
				instruction = json_object_to_json_string_ext(instructionJ, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY);
				
				send(socketfd, instruction, strlen(instruction), 0);
				json_object_put(instructionJ);
				break;
			case 5:
				char getuserinfo[200];
				bzero(buff_out, BUFFER_SZ);
				printf("Nombre del usuario para obtener su informacion: ");
				scanf("%s", &getuserinfo);
				json_object_object_add(instructionJ, "request", json_object_new_string("GET_USER"));
				json_object_object_add(instructionJ, "body", json_object_new_string(getuserinfo));
				
				instruction = json_object_to_json_string_ext(instructionJ, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY);
				
				send(socketfd, instruction, strlen(instruction), 0);
				json_object_put(instructionJ);   
				
				receive = recv(socketfd, buff_out, BUFFER_SZ, 0);		
				if(receive > 0){
					json_instruction = buff_out;
					struct json_object *response = json_object_new_object();
					response = json_tokener_parse(json_instruction);
					
					struct json_object *code;
					json_object_object_get_ex(response, "code", &code);
					rcode = json_object_get_int(code);
					
					if(rcode == 102){						
						printf("\nEse usuario no existe");
					}else{
						struct json_object *body;
						json_object_object_get_ex(response, "body", &body);
						
						StatusFromIU = json_object_get_string(json_object_array_get_idx(body, 1));
						printf("\nUsuario:%s\nEstado:%s", getuserinfo, StatusFromIU);
					}					
				}				         
				break;
			case 6:
				// Lista de instrucciones de la opción 6                
			              
				break;
			case 7:  
				json_object_object_add(instructionJ, "request", json_object_new_string("END_CONEX"));				
				instruction = json_object_to_json_string_ext(instructionJ, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY);
				
				send(socketfd, instruction, strlen(instruction), 0);
				close(socketfd); 
				repetir = 0;
				break;
			default:
				printf("Opcion Inexistente...");
		}	
	}
	return 0;
}