#include "./utils.h"

Cliente *clientes[MAX_CON];
_Atomic unsigned int clienteInc = 0;

Sala *salas[MAX_SALAS];
static _Atomic unsigned int idSala = 1;
static _Atomic unsigned int salasInc = 0;

static pthread_mutex_t lock;
static pthread_t idThread;

void _cria_sala(char salaNome[], Cliente *cliente) {
	pthread_mutex_lock(&lock);

	Sala *sala = (Sala *)malloc(sizeof(Sala));

	sala->idSala = idSala++;
	salaNome[strlen(salaNome)-1] = '\0'; 
	strcpy(sala->salaNome, salaNome);
	
	for(int i = 0; i < MAX_SALAS; ++i){
		if(!salas[i]) {
			salas[i] = sala;
			break;
		}
	}

	salasInc++;
	printf("> Sala \"%s\"[id: %d] criada pelo usuário %s.\n", sala->salaNome, sala->idSala, cliente->nomeUsuario);

	pthread_mutex_unlock(&lock);
}

void *_mostra_salas_existentes(void *cliente_t) {
	Cliente *cliente = (Cliente *)cliente_t;

	for(int i = 0; i < MAX_SALAS; ++i){
		if(!salas[i])
			continue;

		char nomeSala[64];
		char buffer[128];
		bzero(nomeSala, 64);
		
		if(salas[i]) {
			strcpy(nomeSala, salas[i]->salaNome);
			snprintf(buffer, sizeof(buffer), "%s\n", nomeSala);

			if(write(cliente->sockfd, buffer, strlen(buffer)) < 0) {
					perror("ERRO: Falha ao escrever no descritor de arquivo.\n");
					break;
			}
		}
	}

	pthread_detach(pthread_self());

	return NULL;
}

void _seleciona_sala(Cliente *cliente, int opSelecao) {
	while(1) {
		if(opSelecao == 0)
			break;
		else
			cliente->idSala = opSelecao;

		if(cliente->idSala != 0) break;
	}
}

void *recepciona_cliente(void *cliente_t) {
	clienteInc++;
	Cliente *cliente = (Cliente *)cliente_t;
	
	char nomeUsuario[32];
	recv(cliente->sockfd, nomeUsuario, 32, 0);

	strcpy(cliente->nomeUsuario, nomeUsuario);	
	printf("> Usuário %s conectou-se: ", cliente->nomeUsuario);
	printa_addr(cliente->sockaddr);

	cliente->idSala = 0;

	while(1) {
		char opSelecao[4];
		bzero(opSelecao, 4);
		recv(cliente->sockfd, opSelecao, 4, 0);

		if(atoi(opSelecao) == 1) {
			if(salasInc == MAX_SALAS) {
				const char *mensagemMaxSalas = "Não é possível criar mais salas neste servidor!\n";
				
				if(write(cliente->sockfd, mensagemMaxSalas, strlen(mensagemMaxSalas)) < 0) {
					perror("ERRO: Falha ao escrever no descritor de arquivo.\n");
				}

				continue;
			}

			char nomeSala[64];
			recv(cliente->sockfd, nomeSala, 64, 0);
			_cria_sala(nomeSala, cliente);
		} else if(atoi(opSelecao) == 2) {
			pthread_create(&idThread, NULL, &_mostra_salas_existentes, (void*)cliente);
			
			char opSelecao[4];
			bzero(opSelecao, 4);
			recv(cliente->sockfd, opSelecao, 4, 0);

			_seleciona_sala(cliente, atoi(opSelecao));
		}

		if(cliente->idSala != 0) break;
	}

	pthread_detach(pthread_self());

	pthread_create(&idThread, NULL, &_escuta_cliente, (void*)cliente);

	return NULL;
}

void *_escuta_cliente(void *cliente_t) {
	Cliente *cliente = (Cliente *)cliente_t;

	printf("> Escutando usuário %s na sala [id: %d].\n", cliente->nomeUsuario, cliente->idSala);

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