#include "./utils.h"

#define LENGTH 2048

// Global variables
volatile sig_atomic_t flag = 0;
int sockfd = 0;
char nome[16];

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

void checarNome(){
    // char nome[20] = {0};
    // size_t len = 0;

    // for (;;) {  //loop continuo até input correto
    //     fputs ("Digite seu nome: ", stdout);   
    //     if (!fgets (nome, 20, stdin)) {   //checa fim do input
    //         fputs ("(Usuário cancelado)\n", stdout);
    //         return 1;
    //     }
        
    //     len = strlen (nome);//pega o tamanho da variável
        
    //     if (len > 20 || *nome == '\n') {  //checa se é maior que o tamanho ou está vazio
    //         fputs ("Erro: O nome de usuário deve conter mais que 2 e menos que 15 caracteres.\n", stderr);
    //         continue;
    //     }
    //     else {  //se o nome estiver ok, tira o \n e termina o loop
    //         if (len && nome[len - 1] == '\n')   // checa se o último caractere é \n
    //             nome[--len] = 0; //sobrescreve \n com 0
    //         break;
    //     }
    // }

    // printf ("\nNome: %s  (%zu chars)\n", nome, len);


    // char nomeIn[15];
    // char nomeOut[11];
    
    // printf("Digite o seu nome: ");
    // scanf("%s", nomeIn);

    // if(strlen(nomeIn) > 2 && strlen(nomeIn) <= 10){
    //     for(int i = 0; i < 10; i++) {
    //         nomeOut[i] = nomeIn[i];
    //     }
    // }else{
    //     printf("O nome de usuário deve conter mais que 2 e menos que 15 caracteres.\n");
    //     //clear terminal
    // }


    // char nomeIn[50];//caracteres extras caso o usuário digite a mais
    // char nomeOut[11];
    // int checarNome = 0;

    // while(!checarNome){
    //     printf("Digite o nome de usuário (máx. 20 caracteres): ");
    //     scanf("%s", nomeIn);

    //     if(nomeIn == 0){
    //         printf("ERRO: Nome de usuário vazio.\n");
    //     }else{
    //         for(int i = 0; i < 10; i++) {
    //             nomeOut[i] = nomeIn[i];
    //         }
    //         checarNome = 1;
    //     }
    // }

}


void enviar_msg() { //envia mensagens ao servidor
    char message[LENGTH] = {};
	char buffer[LENGTH + 16] = {};

    while(1) {
  	    str_sobrescreve_stdout();
        fgets(message, LENGTH, stdin);
        str_remover_quebralinha(message, LENGTH);

        if (strcmp(message, "/s") == 0) {//se o usuário escrever 'sair' no fim da mensagem, termina o programa
			break;
        } else {
            sprintf(buffer, "%s: %s\n", nome, message);//se não, adiciona o nome do usuário ao início da mensagem
            send(sockfd, buffer, strlen(buffer), 0);//e envia a mensagem
        }

		bzero(message, LENGTH);
        bzero(buffer, LENGTH + 16);
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