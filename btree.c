#include <stdio.h>
#include <string.h>

int cliParser(int argc, char *argv[], FILE *fp, int *registerType){
    int validate = 0;

    if (argc != 4){
        printf("Uso: %s nome_arquivo -r 1 ou 2\n\n1 - Indica Registros de tamanho variavel\n2 - Indica Registros de tamanho fixo\n\n", argv[0]);
        validate = 0;
    } else{
        fp = fopen(argv[1], "r" );

        if (fp == NULL){
            printf("\nErro ao abrir arquivo, encerrando...\n\n");
            validate = 0;
        } else {
            if(strcmp(argv[2],"-r")){
                printf("\nErro! Parametro nao reconhecido, voce quis dizer '-r' ?\n\n");
                validate = 0;
            } else {
                if (strcmp(argv[3],"1") == 0){
                    printf("\nCarregando B-Tree de registros de tamanho variavel...\n\n");
                    *registerType = 1;
                } else if(strcmp(argv[3],"2") == 0){
                    printf("\nCarregando B-Tree de registros de tamanho fixo...\n\n");
                    *registerType = 2;
                } else {
                    printf("\nErro! Valor de %s precisa ser 1 ou 2!\n\n", argv[2]);
                    validate = 0;
                }
            }
        }
    }
    return validate;
}

void loadFixedBTree(){

}

void loadVariableBTree(){

}

int main(int argc, char *argv[]){
    FILE *fp = NULL;
    int registerType = 0;

    int cliCheck = cliParser(argc, argv, fp, &registerType);


    if (cliCheck == 1){
        if (registerType == 1){
            loadVariableBTree();
        } else if(registerType == 2) {
            loadFixedBTree();
        } else {
            printf("Erro inesperado!");
        }
    }


    return 0;
}
