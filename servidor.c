#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>


int main(int argc, char **argv[])
{
        
    if(argc > 1)//debemos recibir 2 parametros sino F amix
    {

        //se definen variables
        int fd, fd2, longitud_cliente, puerto;
        puerto = atoi(argv[1]);

        //se necesitan dos struckt de tipo sockaddr para info del server y para el cliente 
        struct sockaddr_in server;
        struct sockaddr_in client;

        //configurarcion del servidor
        //tengo que corregir la estructura
        server.sin_family = AF_INET;//para el tcp/ip
        server.sin_port = htons(puerto);//puerto 
        server.sin_addr.saddr = INADDR_ANY; //conexion con cualquier cliente
        //bzero(&(server.sin_zero),8); rellenar 0s

        //definimos el socket aun no lo asoscio a un puerto
        if ((fd = socket(AF_INET, SOCK_STREAM,0))<0){
            perror("Error apertura de socket \n")
            exit(-1);
        }
        //avisar al sistema que se creo un socket
        if (bind(fd,(struct sockaddr*)&server, sizeof(struct sockaddr))==-1){
            perror("Error en bind() \n")
            exit(-1);
        }
        //socket en modo escucha
        if (listen(fd,5)== -1){
            printf("error en listen() \n");
            exit(-1);
        }

        //aceptar conexiones
        while(1){
            longitud_cliente = sizeof(struct sockaddr_in);
            if ((fd2 = accept(fd,(struct sockaddr *)&client, &longitud_cliente ))== -1 )
            printf("error en accept() \n")
            exit(-1);
            }
            send(fd2, "bienvenido a mi servidor \n". 26, 0);
            close(fd2);
        }
        close(fd);


    }else{
        printf("No se ingreso el puerto por parametro\n");
    }
return 0;
}
