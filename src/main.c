#include <stdio.h>
#include <strings.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "./constantes.h"

int main(int arc, char **argv) {
    struct sockaddr_in server_addr = {
        .sin_family         = AF_INET,
        .sin_addr.s_addr    = htonl(INADDR_ANY),
        .sin_port           = htons(PORTA)
    };

    int server = socket(AF_INET, SOCK_STREAM, 0);

    bind(server, (struct sockaddr_in *) &server_addr, sizeof server_addr);

    listen(server, MAX_CON);

    struct sockaddr_in client_addr;
    uint client_addr_len = sizeof client_addr;
    int client;
    char buffer[BUFFER_SIZE];

    while(1) {
        bzero(&buffer, BUFFER_SIZE);
        client = accept(server, (struct sockaddr_in *) &client_addr, &client_addr_len);

        recv(client, buffer, sizeof buffer, 0);

        printf("CLIENTE: %s", buffer);

        close(client);
    }

    return 0;
}