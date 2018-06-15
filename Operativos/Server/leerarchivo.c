
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
// Ejemplo aprenderaprogramar.com



int main()
{
    FILE* fichero;
    fichero = fopen("fichero1.txt", "wt");
    fputs("Aprender a programar (linea 1)\n", fichero);
    fputs("requiere esfuerzo (linea 2)\n", fichero);
    fputs("y dedicacion (linea 3)", fichero);
    fclose(fichero);
    printf("Proceso completado");
    return 0;
}
