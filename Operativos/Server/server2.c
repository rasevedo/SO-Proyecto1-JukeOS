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













// ESE SERVER ESCUCHA LA CANCION QUE QUIERE EL CLIENTE Y LA MANDA AL SERVER , PUERTO 8888






/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[2000];



    //Send some messages to the client
    message = "Greetings! I am your connection handler\n";
    write(sock , message , strlen(message));

    message = "Now type something and i shall repeat what you type \n";
    write(sock , message , strlen(message));
    //puts(client_message);
    //Receive a message from client
  //  while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    //{
        //Send the message back to client
        read_size = recv(sock , client_message , 2000 , 0);
        write(sock , client_message , strlen(client_message));
        puts(client_message);
        puts("Abriendo la conexion");
        //sendFiles(client_message);                       //////////////////////////////////////////////7
    //}

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
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);


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

extern int a;
printf("V9\n");
listenClient();

//listSongs();
//sendTxt();
//  sendFiles("TheTrooper.mp3");

    return 0;
}
