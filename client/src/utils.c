#include "./utils.h"

#define LENGTH 2048

// Global variables
volatile sig_atomic_t flag = 0;
int sockfd = 0;
char nome[16];
char salaNome[16];
char opMenu[4];
char idSala[5];
int escolheuSala = 0;



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

void recebe_nome(){                                                  //recebe o nome do usuario e confere se tem o tamanho correto
    int checaNome = 0;
    
    system("clear");
    
    while(!checaNome){
        printf("Digite o seu nome de usuário: ");
        fgets(nome, 16, stdin);
        str_remover_quebralinha(nome, strlen(nome));

        if (strlen(nome) > 16 || strlen(nome) < 2){
		    printf("O nome de usuário deve conter mais que 2 e menos que 15 caracteres.\n");
	    }else{
            send(sockfd, nome, 16, 0);
            checaNome = 1;
        }
    }
}

void mostra_menu(){
    system("clear");
	printf("\n\n======== MENU ========\n");
	printf("1 - Criar nova sala\n");
	printf("2 - Ver salas existentes\n\n");
	printf("Digite a opção desejada: ");
	fgets(opMenu, 2, stdin);
    system("clear");
}

void criar_sala(){
    int checaSala = 0;

    while(!checaSala){
        printf("===== CRIAR NOVA SALA =====\n");
        printf("Digite o nome da sala: ");
        fgets(salaNome, 16, stdin);

        if (strlen(salaNome) > 16 || strlen(salaNome) < 2){
		    printf("\nO nome da sala deve conter mais que 2 e menos que 15 caracteres.\n");
	    }else{
            send(sockfd, salaNome, sizeof(salaNome), 0);
            printf("SALA CRIADA: %s", salaNome);
            checaSala = 1;
            system("clear");
        }
    }
}

void mostra_salas(){
    
    char qtSalas[4];
    char listaSalas[128];
    bzero(listaSalas, 128);
    //getchar();
    printf("===== ESCOLHA A SALA =====\n");
    
    recv(sockfd, qtSalas, sizeof(qtSalas), 0);
        
    if(atoi(qtSalas) != 0) {
        recv(sockfd, listaSalas, 128, 0);
        for(int i = 0; i < atoi(qtSalas); i++){
            printf("%s\n", listaSalas);
        }
        printf("Digite o id da sala desejada (0 para voltar ao menu): ");
        fgets(idSala, 16, stdin);
        str_remover_quebralinha(idSala, strlen(idSala));
    }else{
        printf("Ainda não existem salas neste servidor.\n");
    }
    
    if(strcmp(idSala, "0") != 0){
        send(sockfd, idSala, 5, 0); 
        escolheuSala = 1;
    }else if(strcmp(idSala, "0") == 0){
        send(sockfd, idSala, 5, 0); 
        mostra_menu();
    }else{ 
        printf("Opção inválida!\n\n");
        sleep(2);
        system("clear");
    }
}

void enviar_msg() { //envia mensagens ao servidor
    char message[LENGTH] = {};
	char buffer[LENGTH + 32] = {};

    while(1) {
  	    str_sobrescreve_stdout();
        fgets(message, LENGTH, stdin);
        str_remover_quebralinha(message, LENGTH);

        if (strcmp(message, "/s") == 0) {//se o usuário escrever '/s' no fim da mensagem, termina o programa
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