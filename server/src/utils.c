#include "./utils.h"

int criaSocket() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
   
    if(fd < 0)
        printf("Falha ao criar socket.\n");
    else
        printf("Socket criado com sucesso.\n");

    return fd;
}

struct sockaddr_in * criaEndereco() {
    struct sockaddr_in *addr = malloc(sizeof(struct sockaddr_in));
    addr->sin_family = AF_INET;
    addr->sin_port = htons(PORTA);
    addr->sin_addr.s_addr = INADDR_ANY;

    return addr;
}