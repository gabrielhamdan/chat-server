#include "./utils.h"																		//inclui libs e assinaturas

static int id = 10;																			//var p/ guardar ids únicos de usuários
extern _Atomic unsigned int clienteInc;

int main(int argc, char **argv) {
	printf("> Inicializando o servidor.\n");												//log de inicialização do server

	int serverFd = 0, connfd = 0;															//var p/ fds do server e do cliente

	struct sockaddr_in serv_addr;															//struct do socket do servidor
	struct sockaddr_in cli_addr;															//struct do socket do cliente
	
	static pthread_t idThread;																//id p/ threads

	serverFd = socket(AF_INET, SOCK_STREAM, 0);												//cria socket do server (ipv4, tipo stream, protocolo)

	if(serverFd < 0) {																		//verifica se houve erro na criação do socket e registra resultado no console
		perror("ERRO: Falha ao criar o socket.\n");
		return EXIT_FAILURE;
	} else
		printf("> Socket criado com sucesso.\n");

	if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {		//verifica se foi possível configurar reutilização do socket num endereço criado anteriormente
    	perror("> Falha ao executar setsockopt(SO_REUSEADDR).\n");
		return EXIT_FAILURE;
	}

	serv_addr.sin_family = AF_INET;															//atribui ao endereço do server o tipo de ip
	inet_pton(AF_INET, LOCALHOST, &(serv_addr.sin_addr));									//define endereço (localhost, neste caso)
	serv_addr.sin_port = htons(PORTA);														//define porta de escuta (30000, neste caso)

	if(bind(serverFd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {				//verifica se foi possível vincular o socket
		perror("ERRO: Falha ao vincular o socket.\n");
		return EXIT_FAILURE;
	}

	if (listen(serverFd, MAX_FILA) < 0) {													//inicia a escuta do socket, definindo máximo de conexões possíveis antes de começar a recusá-las
		perror("ERRO: Falha ao escutar o socket.\n");
		return EXIT_FAILURE;
	} else
		printf("> Escutando na porta %d.\n", PORTA);										//em caso de falha ou de sucesso, registra no console

	printf("> Servidor inicializado com sucesso.\n");

	while(1){
		printf("> Aguardando conexões...\n");												//entra num laço infinito, aguardando conexões

		socklen_t clienteLen = sizeof(cli_addr);											//var que guarda o tamanho do socket do cliente
		connfd = accept(serverFd, (struct sockaddr*)&cli_addr, &clienteLen);				//fd do cliente recebe o aceite de conexão

		if((clienteInc + 1) == MAX_CON){													//testa se limite de conexões no servidor já foi atingido
			printf("> Rejeitado: ");
			printa_addr(cli_addr);
			printf("> O número máximo de conexões foi atingido.\n");
			close(connfd);
			continue;																		//caso seja verdadeira a condição, recusa cliente e volta ao topo do laço, printando no console o endereço rejeitado
		}

		Cliente *cliente = (Cliente *)malloc(sizeof(Cliente));								//aloca memória para ponteiro
		cliente->sockaddr = cli_addr;														//atribui à estrutura do cliente o endereço da conexão
		cliente->sockfd = connfd;															//atribui à estrutura do cliente o fd da conexão
		cliente->idUsuario = id++;															//atribui à estrutura do cliente um id e, sem seguida, o incrementa

		adiciona_cliente(cliente);															//chama função para inclusão de clientes
		pthread_create(&idThread, NULL, &recepciona_cliente, (void*)cliente);				//inicia thread para recepcionar novos clientes
	}

	return EXIT_SUCCESS;
}
