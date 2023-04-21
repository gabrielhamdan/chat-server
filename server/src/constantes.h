#ifndef __CONSTANTES_H
#define __CONSTANTES_H

#include <sys/socket.h>

#define PORTA 30000
#define LOCALHOST "127.0.0.1"
#define MAX_CON 10
#define MAX_FILA 10
#define BUFFER_SIZE 1024

typedef struct{
	struct sockaddr_in sockaddr;
	int sockfd;
	int idUsuario;
	char nomeUsuario[32];
} Cliente;

#endif