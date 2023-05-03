#include "./utils.h"																			//inclui libs e assinaturas
										
extern volatile sig_atomic_t flag;																//variaveis globais
extern int sockfd;																				//
extern char nome[16];																			//define o tamanho da variavel nome
extern char salaNome[16];																		//define o tamanho da variavel sala
extern char opMenu[4];
extern char idSala[5];	
extern int escolheuSala;																		//var para fechar o loop

int main(int argc, char **argv){
	// signal(SIGINT, captura_ctrlc_sai);

	//recebe_nome();

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

	recebe_nome();																				//coleta e envia nome para o servidor
		
	while(1){
		mostra_menu();

		if(strcmp(opMenu, "1") == 0){
			send(sockfd, opMenu, sizeof(opMenu), 0);
			criar_sala();

	
		}else if(strcmp(opMenu, "2") == 0){
			send(sockfd, opMenu, sizeof(opMenu), 0); 
			mostra_salas();
			
			// char idSala[5];
			// char qtSalas[4];
			// char listaSalas[128];
			// bzero(listaSalas, 128);
			// //getchar();
			// printf("===== ESCOLHA A SALA =====\n");
			
			// recv(sockfd, qtSalas, sizeof(qtSalas), 0);
				
			// if(atoi(qtSalas) != 0) {
			// 	recv(sockfd, listaSalas, 128, 0);
			// 	for(int i = 0; i < atoi(qtSalas); i++)
			// 		printf("%s\n", listaSalas);
			// } else
			// 	printf("Ainda não existem salas neste servidor.\n");

			// printf("Digite o id da sala desejada (0 para voltar ao menu): ");
			// fgets(idSala, 16, stdin);
			// str_remover_quebralinha(idSala, strlen(idSala));
			// send(sockfd, idSala, 5, 0); 
			
			// if(strcmp(idSala, "0") != 0)
			// 	escolheuSala = 1;
			// } else{ 
			// 	printf("Opção inválida!\n\n");
			// 	sleep(2);
			// 	system("clear");
			// }
		}
		
		if(escolheuSala){break;}
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