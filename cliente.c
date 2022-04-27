#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <json/json.h>

int main()
{
	int opcion;
	int estado;
	int repetir = 1;
	int regresar = 0;
	int result;
	char username2chat[200];
	char mensaje[8000];
	char salir[] = "salir";

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