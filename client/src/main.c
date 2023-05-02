#include "./utils.h"																			//inclui libs e assinaturas
										
extern volatile sig_atomic_t flag;																//variaveis globais
extern int sockfd;																				//
extern char nome[16];																			//define o tamanho da variavel nome
extern char salaNome[16];																		//define o tamanho da variavel sala
char opMenu[4];																	

int main(int argc, char **argv){
	// signal(SIGINT, captura_ctrlc_sai);

	recebe_nome();

	system("clear");
	
	struct sockaddr_in server_addr;

	// config do socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(LOCALHOST);
    server_addr.sin_port = htons(PORTA);

    int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));			//conecta com o servidor
    if (err == -1) {
		printf("ERRO: Falha na conexão\n");
		return EXIT_FAILURE;
	}

	send(sockfd, nome, 16, 0);																	//envia nome para o servidor
	int escolheuSala = 0;
		
	while(1){
		system("clear");
		printf("\n\n======== MENU ========\n");
		printf("1 - Criar nova sala\n");
		printf("2 - Ver salas existentes\n\n");
		printf("Digite a opção desejada: ");
		fgets(opMenu, 2, stdin);

		system("clear");

		if(strcmp(opMenu, "1") == 0){
			send(sockfd, opMenu, sizeof(opMenu), 0);  
			// getchar();
			criar_sala();
			printf("SALA CRIADA: %s", salaNome);
			send(sockfd, salaNome, 16, 0);
			send(sockfd, salaNome, sizeof(salaNome), 0);
			system("clear");
		}else if(strcmp(opMenu, "2") == 0){
			send(sockfd, opMenu, sizeof(opMenu), 0); 
			char idSala[5];
			char qtSalas[4];
			char listaSalas[128];
			bzero(listaSalas, 128);
			// getchar();
			printf("===== ESCOLHA A SALA =====\n");
			
			recv(sockfd, qtSalas, sizeof(qtSalas), 0);
				
			if(atoi(qtSalas) != 0) {
				recv(sockfd, listaSalas, 128, 0);

				for(int i = 0; i < atoi(qtSalas); i++)
					printf("%s\n", listaSalas);
			} else
				printf("Ainda não existem salas neste servidor.\n");

			printf("0 - Voltar ao menu\n\n");
			printf("Digite o id da sala ou 0: ");
			fgets(idSala, 16, stdin);
			str_remover_quebralinha(idSala, strlen(idSala));
			send(sockfd, idSala, 5, 0); 
			
			if(strcmp(idSala, "0") != 0)
				escolheuSala = 1;
		} else{ 
			printf("Opção inválida!\n\n");
			sleep(2);
			system("clear");
		}

		if(escolheuSala) break;
	}
	
	pthread_t send_msg_thread;
    if(pthread_create(&send_msg_thread, NULL, (void *) enviar_msg, NULL) != 0){
		printf("ERRO: Falha ao enviar mensagem\n");
        return EXIT_FAILURE;
	}

	pthread_t recv_msg_thread;
    if(pthread_create(&recv_msg_thread, NULL, (void *) receber_msg, NULL) != 0){
		printf("ERRO: Falha ao receber mensagem\n");
		return EXIT_FAILURE;
	}

	while (1){
		if(flag){
			printf("\nSaiu\n");
			break;
    	}
	}

	close(sockfd);

	return EXIT_SUCCESS;
}