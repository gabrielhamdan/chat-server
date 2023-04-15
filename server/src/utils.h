#ifndef UTILS_H
#define UTILS_H

#include <malloc.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "./constantes.h"

int criaSocket();
struct sockaddr_in * criaEndereco();

#endif