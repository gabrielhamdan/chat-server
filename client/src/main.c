#include "./utils.h"																			//inclui libs e assinaturas
										
extern volatile sig_atomic_t flag;																//variaveis globais
extern int sockfd;																				//
extern char nome[16];																			//define o tamanho da variavel nome
extern char salaNome[16];																			//define o tamanho da variavel sala
//extern int opMenu;

#define LENGTH 2048																				


int main(int argc, char **argv){

	char *ip = "127.0.0.1";																		//define o ip
	int port = 30000;																			//define a porta

	signal(SIGINT, captura_ctrlc_sai);

	recebe_nome();
	
	struct sockaddr_in server_addr;

	// config do socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));			//conecta com o servidor
    if (err == -1) {
		printf("ERRO: Falha na conexão\n");
		return EXIT_FAILURE;
	}

	send(sockfd, nome, 16, 0);																	//envia nome para o servidor

	//menu();
	char opMenu[2];
	int fechaLoop = 0;
		
	while(!fechaLoop){
		printf("\n\n======== MENU ========\n");
		printf("1 - Criar nova sala\n");
		printf("2 - Ver salas existentes\n\n");
		printf("Digite a opção desejada: ");
		//fgets(opMenu, 2, stdin);
		//str_remover_quebralinha(opMenu, strlen(opMenu));
		scanf("%s", opMenu);
		getchar();
		send(sockfd, opMenu, sizeof(opMenu), 0); 
		system("clear");

		if(opMenu == '1'){ 
			criar_sala();
			send(sockfd, salaNome, 16, 0);
			fechaLoop = 0;
			system("clear");
		}else if(opMenu == '2'){
			char idSala[5];
			char listaSalas[LENGTH] = {};
			bzero(listaSalas, LENGTH);
			printf("===== ESCOLHA A SALA =====\n");
			recv(sockfd, listaSalas, LENGTH, 0);
			printf("0 - Voltar ao menu\n\n");
			printf("Digite o id da sala ou 0: ");
			fgets(idSala, 16, stdin);
			str_remover_quebralinha(idSala, strlen(idSala));
			send(sockfd, idSala, 5, 0); 
		}else{ 
			printf("Opção inválida!\n\n");
			fechaLoop = 0;
			sleep(2);
			system("clear");
		}
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