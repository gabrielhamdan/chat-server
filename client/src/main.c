#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    printf("\nCliente inicializado.");

    const char *hostName = "127.0.0.1";
    const char *portNumber = "30000";

    int csocket;

    struct addrinfo addrInfo;
    memset(&addrInfo, 0, sizeof(struct addrinfo));

    struct addrinfo *results;
    struct addrinfo *record;

    addrInfo.ai_family = AF_INET;
    addrInfo.ai_socktype = SOCK_STREAM;
    addrInfo.ai_protocol = IPPROTO_TCP;

    if(getaddrinfo(hostName, portNumber, &addrInfo, &results) != 0) {
        perror("\nFalha ao resolver socket cliente.");
        exit(EXIT_FAILURE);
    }

    printf("\nCliente resolvido.");

    for(record = results; record != NULL; record = record->ai_next) {
        csocket = socket(record->ai_family, record->ai_socktype, record->ai_protocol);

        if(csocket == -1)
            continue;
        
        if(connect(csocket, record->ai_addr, record->ai_addrlen) != -1)
            break;

        close(csocket);
    }

    if(record == NULL) {
        perror("\nFalha ao criar ou conectar socket cliente!");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(results);

    printf("\nSocket cliente conectado com sucesso.");

    char *message = "Mensagem enviada do socket cliente.";

    if(send(csocket, message, strlen(message), 0) == -1) {
        perror("\nFalha ao enviar mensagem!");
        exit(EXIT_FAILURE);
    } else
        printf("\nMensagem enviada com sucesso\n.");
    
    close(csocket);

    return 0;
}