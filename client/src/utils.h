#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define LOCALHOST "127.0.0.1"
#define PORTA 30000
#define STD_LEN 2048

void str_sobrescreve_stdout();
void str_remover_quebralinha (char* arr, int length);
void captura_ctrlc_sai(int sig);
//void recebe_nome();
//void menu();
//void criar_sala();
//void ver_salas();
void enviar_msg();
void receber_msg();

#endif