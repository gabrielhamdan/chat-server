#include "./utils.h"

static Cliente *clientes[MAX_CON];
_Atomic unsigned int clienteInc = 0;

static Sala *salas[MAX_SALAS];
_Atomic unsigned int idSala = 0;

pthread_mutex_t lock;

void _cria_sala(Cliente *cliente) {
	printf("Aguardando criação de sala.\n");

	char salaNome[64];
	recv(cliente->sockfd, salaNome, 64, 0);
	printf("%s", salaNome);

	pthread_mutex_lock(&lock);

	for(int i = 0; i < MAX_SALAS; ++i){
		if(!salas[i]){
			// salas[i]->salaNome[64] = malloc(64);
			// salas[i]->idSala = malloc(16);
			// strcpy(salas[i]->salaNome[64], salaNome);
			salas[i]->idSala = idSala++;
			cliente->idSala = idSala;
			break;
		}
	}

	pthread_mutex_unlock(&lock);

	_escuta_cliente(&cliente);
}

void _mostra_salas_disponiveis(Cliente *cliente) {
}

void _aguarda_selecao_sala(Cliente *cliente) {
	printf("Aguardando seleção de sala.\n");

	char op[4];
	recv(cliente->sockfd, op, 4, 0);
	printf("%s", op);

	if(atoi(op) == 1)
		_mostra_salas_disponiveis(&cliente);
	else if(atoi(op) == 2)
		_cria_sala(&cliente);
}

void *recepciona_cliente(void *cliente_t) {
	char buffer[BUFFER_SIZE];
	char nomeUsuario[32];

	clienteInc++;
	Cliente *cliente = (Cliente *)cliente_t;

	recv(cliente->sockfd, nomeUsuario, 32, 0);

	strcpy(cliente->nomeUsuario, nomeUsuario);
	sprintf(buffer, "%s! ###\n", cliente->nomeUsuario);
	
	printf("> Usuário %s conectou-se: ", cliente->nomeUsuario);
	printa_addr(cliente->sockaddr);

	char mensagemBoasVindasBuff[50];
	strcpy(mensagemBoasVindasBuff, MSG_BOAS_VINDAS);
	strcat(mensagemBoasVindasBuff, buffer);

	if(write(cliente->sockfd, mensagemBoasVindasBuff, strlen(mensagemBoasVindasBuff)) < 0) {
		perror("ERRO: Falha ao escrever no descritor de arquivo.\n");
	}

	// _aguarda_selecao_sala(&cliente);

	printf("Aguardando seleção de sala.\n");

	char op[4];
	recv(cliente->sockfd, op, 4, 0);
	printf("%s", op);

	// if(atoi(op) == 1)
	// 	_mostra_salas_disponiveis(&cliente);
	// else if(atoi(op) == 2)
	// 	_cria_sala(&cliente);

	char salaNome[64];
	recv(cliente->sockfd, salaNome, 64, 0);
	printf("%s", salaNome);

	pthread_mutex_lock(&lock);

	for(int i = 0; i < MAX_SALAS; ++i){
		if(!salas[i]){
			// salas[i]->salaNome[64] = malloc(64);
			// salas[i]->idSala = malloc(16);
			strcpy(salas[i]->salaNome[64], salaNome);
			salas[i]->idSala = idSala++;
			cliente->idSala = idSala;
			break;
		}
	}

	pthread_mutex_unlock(&lock);
}

void _escuta_cliente(Cliente *cliente) {
	printf("Escutando usuário %s na sala %s.", cliente->nomeUsuario, cliente->idSala);

	int desconectou = 0;

	char buffer[BUFFER_SIZE];
	
	sprintf(buffer, "%s entrou.\n", cliente->nomeUsuario);
	envia_mensagem(buffer, cliente->idUsuario, cliente->idSala);

	bzero(buffer, BUFFER_SIZE); 

	while(!desconectou){
		int rec = recv(cliente->sockfd, buffer, BUFFER_SIZE, 0);

		if (rec > 0){
			if(strlen(buffer) > 0)
				envia_mensagem(buffer, cliente->idUsuario, cliente->idSala);
		} else if (rec == 0 || strcmp(buffer, "/s") == 0){
			sprintf(buffer, "%s saiu.\n", cliente->nomeUsuario);
			printf("%s", buffer);
			envia_mensagem(buffer, cliente->idUsuario, cliente->idSala);
			desconectou = 1;
		} else {
			printf("ERRO: Falha ao comunicar-se com o cliente.\n");
			printf("> Desconectado: ");
			printa_addr(cliente->sockaddr);
			desconectou = 1;
		}

		bzero(buffer, BUFFER_SIZE);
	}

	close(cliente->sockfd);
	remove_cliente(cliente->idUsuario);
	free(cliente);
	clienteInc--;

	pthread_detach(pthread_self());
}

void printa_addr(struct sockaddr_in addr) {
	char _addr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(addr.sin_addr), _addr, INET_ADDRSTRLEN);
	printf("%s:%d\n", _addr, htons(addr.sin_port));
}

void adiciona_cliente(Cliente *cliente) {
	pthread_mutex_lock(&lock);

	for(int i = 0; i < MAX_CON; ++i){
		if(!clientes[i]){
			clientes[i] = cliente;
			break;
		}
	}

	pthread_mutex_unlock(&lock);
}

void remove_cliente(int id) {
	pthread_mutex_lock(&lock);

	for(int i=0; i < MAX_CON; ++i) {
		if(clientes[i]){
			if(clientes[i]->idUsuario == id){
				clientes[i] = NULL;
				break;
			}
		}
	}

	pthread_mutex_unlock(&lock);
}

void envia_mensagem(char *mensagem, int idCliente, int idSala) {
	pthread_mutex_lock(&lock);

	for(int i=0; i<MAX_CON; ++i){
		if(clientes[i]){
			if(clientes[i]->idUsuario != idCliente && clientes[i]->idUsuario == idSala){
				if(write(clientes[i]->sockfd, mensagem, strlen(mensagem)) < 0) {
					perror("ERRO: Falha ao escrever no descritor de arquivo.\n");
					break;
				}
			}
		}
	}

	pthread_mutex_unlock(&lock);
}