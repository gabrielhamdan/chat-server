#include "./utils.h"																									//inclui libs e assinaturas

Cliente *clientes[MAX_CON];																								//array de estrutura Cliente
_Atomic unsigned int clienteInc = 0;																					//variável atômica p/ controle de usuários no server

Sala *salas[MAX_SALAS];																									//array de estrutura de Sala
static _Atomic unsigned int idSala = 1;																					//variável atômica p/ controle de id único p/ sala
static _Atomic unsigned int salasInc = 0;																				//variável atômica p/ controle de no server sala

static pthread_mutex_t lock;																							//trava para semáforos de threads
static pthread_t idThread;																								//id de threads

void _cria_sala(char salaNome[], Cliente *cliente) {
	pthread_mutex_lock(&lock);																							//semáforo fecha para outras threads que tentem executar a função

	Sala *sala = (Sala *)malloc(sizeof(Sala));																			//cria e aloca na memória uma estrutura vazia de sala

	sala->idSala = idSala++;																							//atribui id e incrementa p/ próxima sala criada
	salaNome[strlen(salaNome)-1] = '\0';																				//limpa \n do nome da sala
	strcpy(sala->salaNome, salaNome);																					//atribui à estrutura nome da sala enviado pelo cliente
	sala->usuOn = 0;																									//inicializa quantidade de usuários na sala
	
	for(int i = 0; i < MAX_SALAS; ++i){																					//percorre array de salas
		if(!salas[i]) {																									//busca busca pelo primeiro elemento vazio no array
			salas[i] = sala;																							//insere nova sala no array de salas
			break;																										//sai do laço
		}
	}

	salasInc++;																											//incrementa quantidade de salas existentes no server
	printf("> Sala \"%s\"[id: %d] criada pelo usuário %s.\n", sala->salaNome, sala->idSala, cliente->nomeUsuario);		//printa log no server

	pthread_mutex_unlock(&lock);																						//abre semáforo para que outras threads possam executar a função
}

void *_mostra_salas_existentes(void *cliente_t) {
	Cliente *cliente = (Cliente *)cliente_t;																			//aponta para cliente que chegou como parâmetro

	for(int i = 0; i < MAX_SALAS; ++i){																					//percorre array de salas
		if(!salas[i])																									//busca por elementos vazios no array
			continue;																									//para então ignorá-los

		char nomeSala[64];																								//var p/ guardar nome de cada sala
		bzero(nomeSala, 64);																							//limpa lixo acumulado a cada passada da laço
		char usuOn[16];																									//var p/ guardar a quantidade de usuários online em cada sala
		bzero(usuOn, 16);																								//limpa lixo acumulado a cada passada da laço
		
		char buffer[128];																								//buffer para concatenar dados que vão para o cliente
		bzero(buffer, 128);																								//limpa lixo acumulado a cada passada da laço
		
		if(salas[i]) {																									//percorre array de salas
			strcpy(nomeSala, salas[i]->salaNome);																		//atribui nome da sala da iteração na var nomeSala
			sprintf(usuOn, "%d", salas[i]->usuOn);																		//var usuOn recebe a quantidade de usuários online da respectiva sala

			snprintf(buffer, sizeof(buffer), "%s | ONLINE: %s\n", nomeSala, usuOn);										//concatena no buffer o nome da sala e a quantidade de usuários online

			if(write(cliente->sockfd, buffer, strlen(buffer)) < 0) {													//escreve o buffer no fd do cliente e verifica falha
					perror("ERRO: Falha ao escrever no descritor de arquivo.\n");										//printa no log do server em caso de falha
					break;																								//sai imediatamente do laço em caso de falha
			}
		}
	}

	pthread_detach(pthread_self());																						//encerra thread para o id que a iniciou

	return NULL;																										//retorna ponteiro nulo (?) 
}

void _seleciona_sala(Cliente *cliente, int opSelecao) {
	while(1) {																											//entra em laço infinito
		if(opSelecao == 0)																								//teste opção do usuário
			break;																										//se for 0, sai do laço e encerra função
		else {																											//se não,
			cliente->idSala = opSelecao;																				//atribui ao usuário uma sala
			
			for(int i = 0; i < MAX_SALAS; ++i){																			//percorre array de salas
				if(salas[i]->idSala == cliente->idSala) {																//busca pela sala cujo id seja idêntico ao da sala selecionada pelo usuário
					salas[i]->usuOn++;																					//atualiza quantidade de usuários online na sala
					break;																								//sai do laço imediatamente
				}
			} 
		}

		if(cliente->idSala != 0) break;																					//encerra o laço se o usuário tiver selecionado alguma sala
	}
}

void *recepciona_cliente(void *cliente_t) {
	clienteInc++;																										//incrementa quantidade de usuários no server
	Cliente *cliente = (Cliente *)cliente_t;																			//aponta para cliente que chegou como parâmetro
	
	char nomeUsuario[32];																								//variável que recebe nome do usuário
	recv(cliente->sockfd, nomeUsuario, 32, 0);																			//recebe do cliente nome de usuário

	strcpy(cliente->nomeUsuario, nomeUsuario);																			//atribui ao cliente o nome informado
	printf("> Usuário %s conectou-se: ", cliente->nomeUsuario);															//printa no log do server que determinado usuário se conectou
	printa_addr(cliente->sockaddr);																						//e identifica seu endereço de IP e porta à qual está conectado

	cliente->idSala = 0;																								//inicializa o id da sala da estrutura do cliente

	while(1) {																											//entra em laço infinito
		char opSelecao[4];																								//var para receber input do usuário
		bzero(opSelecao, 4);																							//limpa var
		recv(cliente->sockfd, opSelecao, 4, 0);																			//recebe input do usuário e guarda na var opSelecao

		if(atoi(opSelecao) == 1) {																						//converte char para int e testa valor
			if(salasInc == MAX_SALAS) {																					//se for 1, verifica quantidade de salas já existentes no server
				const char *mensagemMaxSalas = "Não é possível criar mais salas neste servidor!\n";						//ponteiro p/ mensagem ao usuário
				
				if(write(cliente->sockfd, mensagemMaxSalas, strlen(mensagemMaxSalas)) < 0) {							//escreve a mensagem no fd do cliente e verifica falha
					perror("ERRO: Falha ao escrever no descritor de arquivo.\n");										//printa no log do server em caso de falha
				}

				continue;																								//volta ao topo do laço e não cria nova sala
			}

			char nomeSala[64];																							//var que recebe nome de nova sala
			recv(cliente->sockfd, nomeSala, 64, 0);																		//recebe do cliente nome da sala a ser criada
			_cria_sala(nomeSala, cliente);																				//chama função para criação de sala
		} else if(atoi(opSelecao) == 2) {																				//converte char para int e testa valor
			if(salasInc == 0) {
				const char *mensagemSalasInexistentes = "Nenhuma sala foi encontrada!\n";								//ponteiro p/ mensagem ao usuário
				
				if(write(cliente->sockfd, mensagemSalasInexistentes, strlen(mensagemSalasInexistentes)) < 0) {			//escreve a mensagem no fd do cliente e verifica falha
					perror("ERRO: Falha ao escrever no descritor de arquivo.\n");										//printa no log do server em caso de falha
				}

				continue;
			}
			
			pthread_create(&idThread, NULL, &_mostra_salas_existentes, (void*)cliente);									//se for 2, cria thread e chama função para mostrar salas existentes no server
			
			char opSelecao[4];																							//var p/ guardar opção de sala escolhida pelo usuário
			bzero(opSelecao, 4);																						//limpa var
			recv(cliente->sockfd, opSelecao, 4, 0);																		//recebe do cliente opção referente a sala escolhida pelo usuário

			_seleciona_sala(cliente, atoi(opSelecao));																	//chama função para selecionar sala, passando como parâmetro o cliente e a opção do usuário
		}

		if(cliente->idSala != 0) break;																					//encerra o laço se o usuário tiver selecionado alguma sala 
	}

	pthread_detach(pthread_self());																						//encerra thread para o id que a iniciou

	pthread_create(&idThread, NULL, &_escuta_cliente, (void*)cliente);													//cria thread para escutar cliente

	return NULL;																										//retorna ponteiro nulo (?)
}

void *_escuta_cliente(void *cliente_t) {
	Cliente *cliente = (Cliente *)cliente_t;																			//aponta para cliente que chegou como parâmetro

	printf("> Escutando usuário %s na sala [id: %d].\n", cliente->nomeUsuario, cliente->idSala);						//printa no log do server em que sala determinado usuário se conectou

	int desconectou = 0;																								//var p/ controlar a conexão com cliente

	char buffer[BUFFER_SIZE];																							//buffer de mensagens
	
	sprintf(buffer, "%s entrou.\n", cliente->nomeUsuario);																//concatena mensagem de entrada de usuário ao seu nome
	envia_mensagem(buffer, cliente->idUsuario, cliente->idSala);														//dispara mensagem p/ usuários da mesma sala em questão
	bzero(buffer, BUFFER_SIZE); 																						//limpa buffer

	while(!desconectou){																								//verifica conexão com cliente para prosseguir ao laço
		int rec = recv(cliente->sockfd, buffer, BUFFER_SIZE, 0);														//guarda resposta do cliente na variável

		if (rec > 0){																									//testa se houve resposta
			if(strlen(buffer) > 0)																						//testa se há algo no buffer
				envia_mensagem(buffer, cliente->idUsuario, cliente->idSala);											//dispara mensagem do usuário aos demais conectados na mesma sala
		} else if (rec == 0 || strcmp(buffer, "/s\n") == 0){															//testa se não houve resposta ou se o usuário deseja sair
			sprintf(buffer, "%s saiu.\n", cliente->nomeUsuario);														//concatena mensagem ao nome de usuário
			printf("%s", buffer);																						//printa no log saída do usuário
			envia_mensagem(buffer, cliente->idUsuario, cliente->idSala);												//envia mensagem aos demais usuários sobre a saída deste
			desconectou = 1;																							//atribui verdadeiro à var desconectou
		} else {																										//se não, entende-se que houve algum tipo de erro na comunicação
			perror("ERRO: Falha ao comunicar-se com o cliente.\n");														//printa mensagem de erro
			printf("> Desconectado: ");																					//printa log de desconexão
			printa_addr(cliente->sockaddr);																				//e identifica seu endereço de IP e porta da qual cliente se desconectou
			desconectou = 1;																							//atribui verdadeiro à var desconectou
		}

		bzero(buffer, BUFFER_SIZE);																						//limpa buffer
	}

	close(cliente->sockfd);																								//fecha o fd co cliente
	remove_cliente(cliente->idUsuario);																					//chama função para remover cliente
	free(cliente);																										//libera da memória o cliente
	clienteInc--;																										//decrementa variável que controla clientes no server
	pthread_detach(pthread_self());																						//encerra thread para o id que a iniciou

	return NULL;
}

void printa_addr(struct sockaddr_in addr) {
	char _addr[INET_ADDRSTRLEN];																						//var p/ guardar endereço de ip
	inet_ntop(AF_INET, &(addr.sin_addr), _addr, INET_ADDRSTRLEN);														//traduz endereço e guarda na var _addr
	printf("%s:%d\n", _addr, htons(addr.sin_port));																		//printa ip e porta passada por parâmetro na struct do socket
}

void adiciona_cliente(Cliente *cliente) {
	pthread_mutex_lock(&lock);																							//semáforo fecha para outras threads que tentem executar a função

	for(int i = 0; i < MAX_CON; ++i){																					//percorre array de clientes
		if(!clientes[i]){																								//busca pelo primeiro elemento vazio
			clientes[i] = cliente;																						//guarda cliente no array
			break;																										//encerra laço
		}
	}

	pthread_mutex_unlock(&lock);																						//abre semáforo para que outras threads possam executar a função
}

void _atualiza_usuarios_online(int idSala) {
	for(int i = 0; i < MAX_SALAS; ++i){																					//percorre array de salas
		if(salas[i]->idSala == idSala) {																				//busca pela sala cujo id seja idêntico ao da sala recebida como parâmetro
			salas[i]->usuOn--;																							//atualiza quantidade de usuários online na sala
			break;																										//sai do laço imediatamente
		}
	} 
}

void remove_cliente(int id) {
	pthread_mutex_lock(&lock);																							//semáforo fecha para outras threads que tentem executar a função

	for(int i=0; i < MAX_CON; ++i) {																					//percorre array de clientes
		if(clientes[i]){																								//verifica se há um cliente no elemento da iteração
			if(clientes[i]->idUsuario == id){																			//verifica id de usuário do cliente
				_atualiza_usuarios_online(clientes[i]->idSala);															//chama função para atualizar quantidade de usuários na sala
				clientes[i] = NULL;																						//exclui cliente do array
				break;																									//encerra laço
			}
		}
	}

	pthread_mutex_unlock(&lock);																						//abre semáforo para que outras threads possam executar a função
}

void envia_mensagem(char *mensagem, int idCliente, int idSala) {
	pthread_mutex_lock(&lock);																							//semáforo fecha para outras threads que tentem executar a função																						

	for(int i=0; i<MAX_CON; ++i){																						//percorre array de clientes
		if(clientes[i]){																								//verifica se há um cliente no elemento da iteração
			if(clientes[i]->idUsuario != idCliente && clientes[i]->idSala == idSala){									//testa se id de usuário é diferente do cliente que está enviando a mensagem e se cliente está na mesma sala que o remetente
				if(write(clientes[i]->sockfd, mensagem, strlen(mensagem)) < 0) {										//escreve a mensagem no fd do cliente e verifica falha
					perror("ERRO: Falha ao escrever no descritor de arquivo.\n");										//printa no log do server em caso de falha
					break;																								//encerra laço
				}
			}
		}
	}

	pthread_mutex_unlock(&lock);																						//abre semáforo para que outras threads possam executar a função
}