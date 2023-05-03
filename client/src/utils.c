#include "./utils.h"

#define LENGTH 2048

// Global variables
volatile sig_atomic_t flag = 0;
int sockfd = 0;
char nome[16];
char salaNome[16];
//char opMenu;


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
            checaNome = 1;
        }
    }

}

// void menu(){
//     int loopMenu = 0;

//     while(loopMenu == 0){
//         printf("\n\n======== MENU ========\n");
//         printf("1 - Criar nova sala\n");
//         printf("2 - Ver salas existentes\n\n");

//         printf("Digite a opção desejada: ");
//         scanf("%d", &loopMenu);


//         switch(loopMenu){
//             case 1: 
//                 criar_sala();
//                 opMenu = '1';
//                 loopMenu = 1;
//                 system("clear");
//                 break;
//             case 2:
//                 ver_salas();
//                 opMenu = '2';
//                 loopMenu = 1;
//                 system("clear");
//                 break;
//             default: 
//                 printf("Opção inválida!\n\n");
//                 sleep(2);
//                 loopMenu = 0;
//                 system("clear");
//         }
//     }

// }

void criar_sala(){
    int checaSala = 0;

    while(!checaSala){
        printf("===== CRIAR NOVA SALA =====\n");
        printf("Digite o nome da sala: ");
        fgets(salaNome, 16, stdin);
        str_remover_quebralinha(salaNome, strlen(salaNome));

        if (strlen(salaNome) > 16 || strlen(salaNome) < 2){
		    printf("\nO nome da sala deve conter mais que 2 e menos que 15 caracteres.\n");
	    }else{
            checaSala = 1;
        }
    }

    //send(sockfd, opMenu, sizeof(opMenu), 0);            //envia a opção do menu
    //send(sockfd, salaNome, 16, 0);                      //envia o nome da sala

    //system("clear");
    //menu();
}

// void ver_salas(){
//     char idSala[5];
//     char listaSalas[LENGTH] = {};
//     bzero(listaSalas, LENGTH);

//     printf("===== ESCOLHA A SALA =====\n");
//     recv(sockfd, listaSalas, LENGTH, 0);
//     printf("0 - Voltar ao menu\n\n");
//     printf("Digite o id da sala ou 0: ");
//     scanf("%s", idSala);
//     getchar();

//     if(idSala == 0){
//         menu();
//     }else{
//         //send(sockfd, opMenu, sizeof(opMenu), 0);        //envia opção do menu
//         send(sockfd, idSala, 5, 0);                     //envia id sala escolhida
//     }

//     system("clear");   

// }

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











// printf("\n\n======== MENU ========\n");
//     printf("1 - Criar nova sala\n");
//     printf("2 - Ver salas existentes\n\n");
// 	printf("Digite a opção desejada: ");
//     fgets(opMenu, 2, stdin);
// 	op = opMenu - '0';
// 	str_remover_quebralinha(opMenu, strlen(opMenu));
// 	send(sockfd, opMenu, sizeof(opMenu), 0); 
// 	system("clear");

// 	switch(op){
// 		case 1: 
// 			printf("===== CRIAR NOVA SALA =====\n");
//         	printf("Digite o nome da sala: ");
//         	fgets(salaNome, 16, stdin);
//         	str_remover_quebralinha(salaNome, strlen(salaNome));
// 			send(sockfd, salaNome, 16, 0);
// 			system("clear");

// 			printf("\n\n======== MENU ========\n");
// 			printf("1 - Criar nova sala\n");
// 			printf("2 - Ver salas existentes\n\n");
// 			printf("Digite a opção desejada: ");
// 			fgets(opMenu, 2, stdin);
// 			str_remover_quebralinha(opMenu, strlen(opMenu));
// 			send(sockfd, opMenu, sizeof(opMenu), 0); 
// 			system("clear");
// 			break;
// 		case 2:
// 			char idSala[5];
// 			char listaSalas[LENGTH] = {};
// 			bzero(listaSalas, LENGTH);
// 			printf("===== ESCOLHA A SALA =====\n");
// 			recv(sockfd, listaSalas, LENGTH, 0);
// 			printf("0 - Voltar ao menu\n\n");
// 			printf("Digite o id da sala ou 0: ");
// 			fgets(idSala, 16, stdin);
//         	str_remover_quebralinha(idSala, strlen(idSala));
// 			send(sockfd, idSala, 5, 0);   
// 			break;
// 		default: 
// 			printf("Opção inválida!\n\n");
// 			printf("Digite a opção desejada: ");
// 			fgets(opMenu, 2, stdin);
// 			str_remover_quebralinha(opMenu, strlen(opMenu));
// 			send(sockfd, opMenu, sizeof(opMenu), 0); 
//     }