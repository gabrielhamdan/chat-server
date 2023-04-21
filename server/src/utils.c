#include "./utils.h"

static Cliente *clientes[MAX_CON];
_Atomic unsigned int clienteInc = 0;
pthread_mutex_t lock;

void *escuta_cliente(void *cliente_t) {
	char buffer[BUFFER_SIZE];
	char nomeUsuario[32];
	int desconectou = 0;

	clienteInc++;
	Cliente *cliente = (Cliente *)cliente_t;

	recv(cliente->sockfd, nomeUsuario, 32, 0);
	strcpy(cliente->nomeUsuario, nomeUsuario);
	sprintf(buffer, "%s entrou.\n", cliente->nomeUsuario);
	printf("%s", buffer);
	envia_mensagem(buffer, cliente->idUsuario);

	bzero(buffer, BUFFER_SIZE); 

	while(!desconectou){
		int rec = recv(cliente->sockfd, buffer, BUFFER_SIZE, 0);

		if (rec > 0){
			if(strlen(buffer) > 0)
				envia_mensagem(buffer, cliente->idUsuario);
		} else if (rec == 0 || strcmp(buffer, "/s") == 0){
			sprintf(buffer, "%s saiu.\n", cliente->nomeUsuario);
			printf("%s", buffer);
			envia_mensagem(buffer, cliente->idUsuario);
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

	for(int i=0; i < MAX_CON; ++i){
		if(clientes[i]){
			if(clientes[i]->idUsuario == id){
				clientes[i] = NULL;
				break;
			}
		}
	}

	pthread_mutex_unlock(&lock);
}

void envia_mensagem(char *mensagem, int id) {
	pthread_mutex_lock(&lock);

	for(int i=0; i<MAX_CON; ++i){
		if(clientes[i]){
			if(clientes[i]->idUsuario != id){
				if(write(clientes[i]->sockfd, mensagem, strlen(mensagem)) < 0){
					perror("ERRO: Falha ao escrever no descritor de arquivo.\n");
					break;
				}
			}
		}
	}

	pthread_mutex_unlock(&lock);
}