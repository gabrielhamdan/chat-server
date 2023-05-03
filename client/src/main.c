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
			
			//pthread_t mostra_salas_thread;
			mostra_salas();
			//if(pthread_create(&mostra_salas_thread, NULL, (void *) mostra_salas, NULL) != 0){
			//	printf("ERRO: Falha ao escolher a sala");
			//	return EXIT_FAILURE;
			//}			
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