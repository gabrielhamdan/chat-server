#include "./utils.h"

#define LENGTH 2048

// Global variables
volatile sig_atomic_t flag = 0;
int sockfd = 0;
char nome[16];
char sala[16];
int opMenu = 0;
int checaNome = 0;

void str_sobrescreve_stdout() {                                      //sobrescreve o texto do console com '>'
    printf("%s", "> ");
    fflush(stdout);
}

void str_remover_quebralinha (char* arr, int length) {               //remove '\n' do final da string
    int i;
    for (i = 0; i < length; i++) { // remove \n
        if (arr[i] == '\n') {
            arr[i] = '\0';
            break;
        }
    }
}

void captura_ctrlc_sai(int sig) {                                    //quando o ctrl-c for identificado, muda a flag para 1 e sai do programa
    flag = 1;
}

void recebe_nome(){                                                 //recebe o nome do usuario e confere se tem o tamanho correto

    while(!checaNome){
        printf("Digite o seu nome de usuário: ");
        fgets(nome, 16, stdin);
        str_remover_quebralinha(nome, strlen(nome));

        if (strlen(nome) > 16 || strlen(nome) < 2){
		    printf("O nome de usuário deve conter mais que 2 e menos que 15 caracteres.\n");
	    }else{
            checaNome = 1;
        }
    }
	
}

void menu(){
    
    while(!opMenu){
        printf("======== MENU ========\n");
        printf("1 - Criar nova sala\n");
        printf("2 - Ver salas existentes\n");
        scanf("%d", &opMenu);

        switch(opMenu){
            case 1: 
                criar_sala();
                break;
            case 2:
                ver_salas();
                break;
            default: 
                opMenu = 0;
        }
    }

}

void criar_sala(){
    int checaSala = 0;

    while(!checaSala){
        printf("Digite o seu nome da sala: ");
        fgets(sala, 16, stdin);
        str_remover_quebralinha(sala, strlen(sala));

        if (strlen(nome) > 16 || strlen(nome) < 2){
		    printf("O nome da sala deve conter mais que 2 e menos que 15 caracteres.\n");
	    }else{
            checaSala = 1;
        }
    }

    send(sockfd, sala, 16, 0);
}

void ver_salas(){
    int opSalas = 0;

    printf("===== ESCOLHA A SALA =====");
    //loop imprimindo o nome das salas
    printf("0 - Voltar ao menu");
    scanf("%d", &opSalas);
}

void enviar_msg() { //envia mensagens ao servidor
    char message[LENGTH] = {};
	char buffer[LENGTH + 32] = {};

    while(1) {
  	    str_sobrescreve_stdout();
        fgets(message, LENGTH, stdin);
        str_remover_quebralinha(message, LENGTH);

        if (strcmp(message, "/s") == 0) {//se o usuário escrever 'sair' no fim da mensagem, termina o programa
			break;
        } else {
            sprintf(buffer, "%s: %s\n", nome, message);//se não, adiciona o nome do usuário ao início da mensagem
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