#include "./utils.h"

static int id = 10;
extern _Atomic unsigned int clienteInc;

int main(int argc, char **argv) {
	printf("> Inicializando o servidor.\n");

	int serverFd = 0, connfd = 0;

	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	
	pthread_t idThread;

	serverFd = socket(AF_INET, SOCK_STREAM, 0);

	if(serverFd < 0) {
		perror("ERRO: Falha ao criar o socket.\n");
		return EXIT_FAILURE;
	} else
		printf("> Socket criado com sucesso.\n");

	serv_addr.sin_family = AF_INET;
	inet_pton(AF_INET, LOCALHOST, &(serv_addr.sin_addr));
	serv_addr.sin_port = htons(PORTA);

	if(bind(serverFd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("ERRO: Falha ao vincular o socket.\n");
		return EXIT_FAILURE;
	}

	if (listen(serverFd, MAX_FILA) < 0) {
		perror("ERRO: Falha ao escutar o socket.\n");
		return EXIT_FAILURE;
	} else
		printf("> Escutando na porta %d.\n", PORTA);

	printf("> Servidor inicializado com sucesso.\n");

	while(1){
		printf("> Aguardando conexões...\n");

		socklen_t clienteLen = sizeof(cli_addr);
		connfd = accept(serverFd, (struct sockaddr*)&cli_addr, &clienteLen);

		if((clienteInc + 1) == MAX_CON){
			printf("> Rejeitado: ");
			printa_addr(cli_addr);
			printf("> O número máximo de conexões foi atingido.\n");
			close(connfd);
			continue;
		}

		Cliente *cliente = (Cliente *)malloc(sizeof(Cliente));
		cliente->sockaddr = cli_addr;
		cliente->sockfd = connfd;
		cliente->idUsuario = id++;

		adiciona_cliente(cliente);
		pthread_create(&idThread, NULL, &escuta_cliente, (void*)cliente);
	}

	return EXIT_SUCCESS;
}
