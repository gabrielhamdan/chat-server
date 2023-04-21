#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define LENGTH 2048

// Global variables
volatile sig_atomic_t flag = 0;
int sockfd = 0;
char name[32];

void str_sobrescreve_stdout() { //sobrescreve o texto do console com '>'
    printf("%s", "> ");
    fflush(stdout);
}

void str_remover_quebralinha (char* arr, int length) { //remove '\n' do final da string
    int i;
    for (i = 0; i < length; i++) { // remove \n
        if (arr[i] == '\n') {
            arr[i] = '\0';
            break;
        }
    }
}

void captura_ctrlc_sai(int sig) { //quando o ctrl-c for identificado, muda a flag para 1 e sai do programa
    flag = 1;
}

void enviar_msg() { //envia mensagens ao servidor
    char message[LENGTH] = {};
	char buffer[LENGTH + 32] = {};

    while(1) {
  	    str_sobrescreve_stdout();
        fgets(message, LENGTH, stdin);
        str_remover_quebralinha(message, LENGTH);

        if (strcmp(message, "sair") == 0) {//se o usuário escrever 'sair' no fim da mensagem, termina o programa
			break;
        } else {
        sprintf(buffer, "%s: %s\n", name, message);//se não, adiciona o nome do usuário ao início da mensagem
        send(sockfd, buffer, strlen(buffer), 0);//e envia a mensagem
        }

		bzero(message, LENGTH);
        bzero(buffer, LENGTH + 32);
    }
    captura_ctrlc_sai(2);
}

void receber_msg() { //recebe mensagens do servidor e imprime no console
	char message[LENGTH] = {};
    while (1) {
		int receive = recv(sockfd, message, LENGTH, 0);
        if (receive > 0) {
            printf("%s", message);
            str_sobrescreve_stdout();
        } else if (receive == 0) {
			break;
        } else {
			// -1
		}
		memset(message, 0, sizeof(message));
    } 
}

int main(int argc, char **argv){
	if(argc != 2){
		printf("Uso: %s <port>\n", argv[0]);
		return EXIT_FAILURE;
	}

	char *ip = "127.0.0.1";
	int port = atoi(argv[1]);

	signal(SIGINT, captura_ctrlc_sai);

	printf("Digite o seu nome de usuário: ");
    fgets(name, 32, stdin);
    str_remover_quebralinha(name, strlen(name));


	if (strlen(name) > 32 || strlen(name) < 2){
		printf("O nome de usuário deve conter mais que 2 e menos que 30 caracteres.\n");
		return EXIT_FAILURE;
	}

	struct sockaddr_in server_addr;

	/* Socket settings */
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
	send(sockfd, name, 32, 0);

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