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

char *name;
int socketfd = 0;
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
		
		if(strcmp(buffer, "salir") == 0){
			break;
		} else {
			sprintf(message, "%s: %s\n", name, buffer);
			send(socketfd, message, strlen(message), 0);
		}
	bzero(buffer, 200);
	bzero(message, 8000);
	}
	catch_ctrl_c_and_exit(2);
}

int main(int argc, char *argv[])
{
	int opcion;
	int estado;
	int repetir = 1;
	int result;
	char username2chat[200];
	
	char *host;
	int listenfd = 0, connfd = 0;
    	struct sockaddr_in serverAddr;
    	struct sockaddr_in cliAddr;
   	long port;
   	pthread_t tid;
	
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
	
	name = argv[1];
	host = argv[2];
	port = atoi(argv[3]);
	
	signal(SIGINT, catch_ctrl_c_and_exit);
	str_trim_lf(name, strlen(name));
	
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(host);
	serverAddr.sin_port = htons(port);
	
	
	/*
	struct json_object *jobj = json_object_new_object();
	struct json_object *jarray = json_object_new_array();
	struct json_object *jstring1 = json_object_new_string("c");
	struct json_object *jstring2 = json_object_new_string("c++");
	const char *question = "Mum, clouds hide alien spaceships don't they ?";
	const char *answer = "Of course not! (\"sigh\")";
	
	json_object_array_add(jarray,jstring1);
	json_object_array_add(jarray,jstring2);
	json_object_object_add(jobj,"Categories", jarray);
	json_object_object_add(jobj, "question", json_object_new_string(question));
	json_object_object_add(jobj, "answer", json_object_new_string(answer));
	 
	printf("jobj from str:\n---\n%s\n---\n", json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
	
	jobj = json_object_new_object();
	json_object_object_add(jobj,"Categories", jarray);
	printf("jobj from str:\n---\n%s\n---\n", json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
	*/
	
	
	while (repetir == 1) {

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
				socketfd = socket(AF_INET, SOCK_STREAM, 0);
				
				int err = connect(socketfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
				if (err == -1){
					printf("ERROR: connect\n");
					return EXIT_FAILURE;
				}		
				send(socketfd, name, strlen(name), 0);
	
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
				close(socketfd);             
				break;
			case 2:
				// Lista de instrucciones de la opción 2                
				printf("Nombre del usuario para entablar conversacion: ");
				scanf("%s", &username2chat);
				printf("The string you entered is : %s\n", username2chat);
				char username2chat[200];
				break;

			case 3:
				// Lista de instrucciones de la opción 3   
				printf("\nOpciones de status");
				printf("\n1. OCUPADO");
				printf("\n2. ACTIVO");
				printf("\n3. INACTIVO");  
				printf("\nIngrese una opcion: ");           
            			scanf("%d", &estado);
				switch(estado){
					case 1:
						printf("\nAhora su estado es OCUPADO");
						break;
					case 2:
						printf("\nAhora su estado es ACTIVO");
						break;
					case 3:
						printf("\nAhora su estado es INACTIVO");
						break;
					default:
						printf("\nEsa opcion no existe. Volviendo al menu...\n");
						sleep(1);
				}
			case 4:
				// Lista de instrucciones de la opción 4                
               
				break;
			case 5:
				// Lista de instrucciones de la opción 5              
				                
				break;
			case 6:
				// Lista de instrucciones de la opción 6                
			              
				break;
			case 7:
				repetir = 0;
		}	
	}
	return 0;
}