#include <stdio.h>
#include <string.h>
#include "./constantes.h"
#include "./utils.h"

int main() {
    printf("chat-server inicializado.\n");

    int serverFd = criaSocket();
    
    struct sockaddr_in *serverAddr = criaEndereco();

    if(bind(serverFd, serverAddr, sizeof(*serverAddr)) < 0)
        printf("Erro ao vincular socket ao endereço do servidor");
    
    listen(serverFd, MAX_CON);

    const struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    int clientFd = accept(serverFd, &clientAddr, &clientAddrLen);

    char buffer[BUFFER_SIZE];

    while(1) {
        bzero(&buffer, BUFFER_SIZE);
        recv(clientFd, &buffer, BUFFER_SIZE, 0);

        printf("CLIENTE: %s", buffer);
    }

    close(clientFd);
    shutdown(serverFd, SHUT_RDWR);

    return 0;
}