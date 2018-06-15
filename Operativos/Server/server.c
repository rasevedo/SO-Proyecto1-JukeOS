// Para ejecutar: archivo
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
      printf("¡Conexion exitosa, con id: %d!\n",connfd);
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
flock();
//listenClient();   ////////////////////////////////////// Aqui ocupo que cierre el puerto

return 0;
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
    int option = 1;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    if(listenfd<0)
	{
	  printf("Error en la creación del socket\n");
	  exit(2);
	}

    printf("Recuperacion del socket exiosa, voy a enviar: \n");
    printf(cancion);
    

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(8888);

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

}


















/*
 * Handler de la conexion con cliente
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[2000];



    //Send some messages to the client
    //message = "Greetings! I am your connection handler\n";
    //write(sock , message , strlen(message));

    //message = "Now type something and i shall repeat what you type \n";
    //write(sock , message , strlen(message));
    //puts(client_message);
    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        //Send the message back to client
        read_size = recv(sock , client_message , 2000 , 0);
        write(sock , client_message , strlen(client_message));
        puts("mensaje del cliente: ");
        puts(client_message);
	escribirArchivo(client_message);
        puts("Abriendo la conexion");
        
        //leerArchivo();                       ////////////////////////////////7
    }

    return 0;
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

    //Free the socket pointer
    free(socket_desc);

    return 0;
}


// Funcion que recibe la cancion que quiere el cliente
void listenClient(){
     int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
   // puts("Waiting for incoming connections...");
   // c = sizeof(struct sockaddr_in);


    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");

        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;

        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }

        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");

    }


    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }

    return 0;


}



















// Lista las canciones en un archivo txt



void listSongs(){
  // Write the list of songs:
  FILE *fp;
  fp = fopen("/home/yonattan/Documentos/Operativos/Server/songsList.txt", "w+");

  // List the songs
  DIR *d;
  struct dirent *dir;

  // Start reading
  d = opendir("/home/yonattan/Documentos/Operativos/Server/Songs/");
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



int escribirArchivo(char cancion[]){

    FILE* fichero;
    fichero = fopen("fichero1.txt", "wt");
    fputs(cancion, fichero);
    fclose(fichero);
    printf("Proceso completado");
    return 0;
}


void leerArchivo()
{
char c[1000];
    FILE *fptr;

    if ((fptr = fopen("fichero1.txt", "r")) == NULL)
    {
        printf("Error! opening file");
        // Program exits if file pointer returns NULL.
        exit(1);         
    }

    // reads text until newline 
    fscanf(fptr,"%[^\n]", c);

    printf("Data from the file:\n%s", c);
    fclose(fptr);
    sendFiles(c); 
    
/*
 	FILE *fp;
 	
 	char buffer[100];
 	
 	fp = fopen ( "fichero1.txt", "r" );
 	
 	fscanf(fp, "%s" ,buffer);
 	printf("%s",buffer);
 	sendFiles(buffer); 
 	fclose ( fp );
 */
 	
}


int main()
{
extern int a;
printf("Recibiendo peticiones en 8888\n");

listSongs(); //
//escribirArchivo();
//leerArchivo();
//printf(leerArchivo());
//listSongs();
//sendTxt();
listenClient(); //
//sendFiles("TheTrooper.mp3"); //5000

    return 0;
}
