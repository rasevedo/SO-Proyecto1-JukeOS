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
#include <dirent.h>

#define BITS 8

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





// Función para enviar archivos y levantar el server

void sendFiles(char cancion[]){

int connfd = 0,err;
    pthread_t tid;
    struct sockaddr_in serv_addr;
    int listenfd = 0,ret;
    char sendBuff[1025];
    int numrv;
    size_t clen=0;
    char client_message[2000];

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


   strcpy(fname,cancion); //Aqui se manda el archivo

    while(1)
    {
        clen=sizeof(c_addr);
        printf("Esperando...\n");
        printf("00000000000000000000000000");
        connfd = accept(listenfd, (struct sockaddr*)&c_addr,&clen);

//Receive a message from client
recv(connfd, client_message , 2000 , 0);
//Send the message back to client
write(connfd, client_message , strlen(client_message));
printf("00000000000000000000000000");
printf(client_message);



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

}



// Lista las canciones en un archivo txt



void listSongs(){
  // Write the list of songs:
  FILE *fp;
  fp = fopen("/local/home/patronadmin/Documents/temp/V1/songsList.txt", "w+");

  // List the songs
  DIR *d;
  struct dirent *dir;

  // Start reading
  d = opendir("/local/home/patronadmin/Documents/temp/V1/Songs/");
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      printf("%s\n", dir->d_name);
      fputs(dir->d_name, fp);
      fputs("\n",fp);
    }
    closedir(d);
    fclose(fp);
  }

}





int main()
{

  listSongs();
  printf("00000000000000000000000000");
  sendFiles("TheTrooper.mp3");

    return 0;
}
