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

int main(int argc, char *argv[])
{
	int opcion;
	int estado;
	int repetir = 1;
	int regresar = 0;
	int result;
	char username2chat[200];
	char mensaje[8000];
	char salir[] = "salir";
	
	char *name;
    	struct sockaddr_in serverAddr;
    	struct sockaddr_in cliAddr;
    	struct hostent *host;
   	long port;
   	pthread_t tid;
	
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
	
	if(argc != 4)
	{	
		fprintf(stderr, "./client [username] [host] [port]\n");
		exit(1);
	}
	
	name = argv[1];
	if((host = gethostbyname(argv[2])) == NULL)
	{
		fprintf(stderr, "Couldn't get host name\n");
		exit(1);
	}
	
	port = atoi(argv[3];
	if((socketFd = socket(AF_INET, SOCK_STREAM, 0))== -1)
	{
		fprintf(stderr, "Couldn't create socket\n");
		exit(1);
	}
	
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
				// Lista de instrucciones de la opción 1 
				while (regresar == 0){
					printf("Mensaje: ");
					scanf("%s", &mensaje);
					result = strcmp(mensaje, salir);
					if (result == 0){
						regresar = 1;
					}else{
						printf("The string you entered is : %s\n", mensaje);
						char mensaje[8000];
					}
				}
				regresar = 0;                
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
				break;
		}	
	}
	return 0;
}