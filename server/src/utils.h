#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <malloc.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "./constantes.h"

void printa_addr(struct sockaddr_in addr);
void adiciona_cliente(Cliente *cliente);
void remove_cliente(int id);
void envia_mensagem(char *mensagem, int id);
void *escuta_cliente(void *cliente_t);

#endif