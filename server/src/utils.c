#include "./utils.h"

static Cliente *clientes[MAX_CON];
_Atomic unsigned int clienteInc = 0;

static Sala *salas[MAX_SALAS];
_Atomic unsigned int idSala = 0;

pthread_mutex_t lock;
pthread_t idThreadEscutaCliente;

int _cria_sala(char salaNome[]) {
	pthread_mutex_lock(&lock);

	Sala *sala = (Sala *)malloc(sizeof(Sala));

	sala->idSala = idSala++;
	strcpy(sala->salaNome, salaNome);
	
	for(int i = 0; i < MAX_SALAS; ++i){
		if(!salas[i]) {
			salas[i] = sala;
			break;
		}
	}

	pthread_mutex_unlock(&lock);

	return idSala;
}

void _mostra_salas_disponiveis(Cliente *cliente) {
}

// void _aguarda_selecao_sala(Cliente *cliente) {
// 	printf("Aguardando seleção de sala.\n");

// 	char op[4];
// 	recv(cliente->sockfd, op, 4, 0);

// 	int opInt = atoi(op);

// 	if(atoi(opInt) == 1)
// 		_mostra_salas_disponiveis(&cliente);
// 	else if(atoi(op) == 2)
// 		_cria_sala(&cliente);
// }

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

	// char opSelecao[4];
	// recv(cliente->sockfd, opSelecao, 4, 0);
	// _aguarda_selecao_sala(atoi(opSelecao));

	// cria sala
	// Cliente *cliente = (Cliente *)malloc(sizeof(Cliente));
	char nomeSala[64];
	recv(cliente->sockfd, nomeSala, 64, 0);
	int idSala = _cria_sala(nomeSala);

	char testeId[4];
	recv(cliente->sockfd, testeId, 4, 0);
	cliente->idSala = atoi(testeId);


	pthread_detach(pthread_self());

	pthread_create(&idThreadEscutaCliente, NULL, &_escuta_cliente, (void*)cliente);

	return NULL;
}

void *_escuta_cliente(void *cliente_t) {
	Cliente *cliente = (Cliente *)cliente_t;

	printf("> Escutando usuário %s na sala %d.\n", cliente->nomeUsuario, cliente->idSala);

	int desconectou = 0;

	char buffer[BUFFER_SIZE];
	
	sprintf(buffer, "> %s entrou.\n", cliente->nomeUsuario);
	envia_mensagem(buffer, cliente->idUsuario, cliente->idSala);

	bzero(buffer, BUFFER_SIZE); 

	while(!desconectou){
		int rec = recv(cliente->sockfd, buffer, BUFFER_SIZE, 0);

		if (rec > 0){
			if(strlen(buffer) > 0)
				envia_mensagem(buffer, cliente->idUsuario, cliente->idSala);
		} else if (rec == 0 || strcmp(buffer, "/s") == 0){
			sprintf(buffer, "> %s saiu.\n", cliente->nomeUsuario);
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

	return NULL;
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
			if(clientes[i]->idUsuario != idCliente && clientes[i]->idSala == idSala){
				if(write(clientes[i]->sockfd, mensagem, strlen(mensagem)) < 0) {
					perror("ERRO: Falha ao escrever no descritor de arquivo.\n");
					break;
				}
			}
		}
	}

	pthread_mutex_unlock(&lock);
}