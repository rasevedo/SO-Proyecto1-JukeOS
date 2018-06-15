// Para ejecutar: 
// gcc server.c -o server -pthread
// ./server im_my_blood.mp3




#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>


struct sockaddr_in c_addr;
char fname[100];

void* SendFileToClient(int *arg)
{
      int connfd=(int)*arg;
      printf("Coneccion exitosa, con id: %d\n",connfd);
      printf("Conectado al cliente: %s:%d\n",inet_ntoa(c_addr.sin_addr),ntohs(c_addr.sin_port));
       write(connfd, fname,256);

        FILE *fp = fopen(fname,"rb");
        if(fp==NULL)
        {
            printf("Error al abrir el archivo");
            return 1;   
        }   

        /* Read data from file and send it */
        while(1)
        {
            /* First read file in chunks of 256 bytes */
            unsigned char buff[1024]={0};
            int nread = fread(buff,1,1024,fp);
            //printf("Bytes read %d \n", nread);        

            /* If read was success, send data. */
            if(nread > 0)
            {
                //printf("Sending \n");
                write(connfd, buff, nread);
            }
            if (nread < 1024)
            {
                if (feof(fp))
		{
                    printf("Fin de archivo\n");
		    printf("Transferencia de archivo completa para el id: %d\n",connfd);
		}
                if (ferror(fp))
                    printf("Error de lectura\n");
                break;
            }
        }
printf("Cerrando conexion para el id: %d\n",connfd);
close(connfd);
shutdown(connfd,SHUT_WR);
sleep(2);
}





int main(int argc, char *argv[])
{
    int connfd = 0,err;
    pthread_t tid; 
    struct sockaddr_in serv_addr;
    int listenfd = 0,ret;
    char sendBuff[1025];
    int numrv;
    size_t clen=0;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd<0)
	{
	  printf("Error en la creación del socket\n");
	  exit(2);
	}

    printf("Recuperacion del socket exiosa\n");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

    ret=bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));
    if(ret<0)
    {
      printf("Error en el enlace\n");
      exit(2);
    }

    if(listen(listenfd, 10) == -1)
    {
        printf("Fallo al esperar por conexiones\n");
        return -1;
    }

if (argc < 2) 
{
	printf("Ingrese el nombre del archivo a enviar: ");
        gets(fname);
}
else
   strcpy(fname,argv[1]); //Aqui se manda el archivo

    while(1)
    {
        clen=sizeof(c_addr);
        printf("Esperando...\n");
        connfd = accept(listenfd, (struct sockaddr*)&c_addr,&clen);
        if(connfd<0)
        {
	  printf("Error al aceptar el archivo\n");
	  continue;	
	}
        err = pthread_create(&tid, NULL, &SendFileToClient, &connfd);
        if (err != 0)
            printf("\nNo se puede crear el hilo :[%s]", strerror(err));
   }
    close(connfd);
    return 0;
}
