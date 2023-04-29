#include "./utils.h"

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
        // str_remover_quebralinha(message, LENGTH);

        if (strcmp(message, "/s") == 0) {//se o usuário escrever 'sair' no fim da mensagem, termina o programa
			break;
        } else {
            // sprintf(buffer, "%s: %s\n", name, message);//se não, adiciona o nome do usuário ao início da mensagem
            send(sockfd, message, strlen(message), 0);//e envia a mensagem
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