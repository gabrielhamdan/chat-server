#include "./utils.h"

// variáveis globais
extern volatile sig_atomic_t flag;
extern int sockfd;
extern char nome[16];


int main(int argc, char **argv){

	char *ip = "127.0.0.1";
	int port = 30000;
	int checarNome = 0;

	signal(SIGINT, captura_ctrlc_sai);

	// checarNome();

	//não libera a entrada no servidor enquanto o nome não for válido
	while(!checarNome){
		printf("Digite o seu nome de usuário: ");
    	fgets(nome, 16, stdin);
    	str_remover_quebralinha(nome, strlen(nome));

		if(strlen(nome) > 2 && strlen(nome) <= 15){
			checarNome = 1;
		}else{
			printf("O nome de usuário deve conter mais que 2 e menos que 15 caracteres.\n");
			//clear terminal
		}
	}
	
	struct sockaddr_in server_addr;

	// config do socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);


    // Conectar com servidor
    int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err == -1) {
		printf("ERRO: Falha na conexão\n");
		return EXIT_FAILURE;
	}

	// Enviar nome
	send(sockfd, nome, 16, 0);

	printf("=== BEM-VINDE À SALA ===\n");

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