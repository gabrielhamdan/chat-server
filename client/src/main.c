#include "./utils.h"																			//inclui libs e assinaturas
										
extern volatile sig_atomic_t flag;																//Variaveis globais
extern int sockfd;																				//
extern char nome[16];
extern char sala[16];
extern int opMenu;																				//define o tamanho da variavel nome


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
	//send(sockfd, sala, 16, 0);

	menu();	

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