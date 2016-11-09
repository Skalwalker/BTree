#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define M 5

FILE *fp;

typedef struct chave{
    int ident;
    char chave[8];
    int prr; /* Qtd de prr, 4*/
}t_chave;

typedef struct no{
    int contador; /* Conta a contidade de chaves, 4 */
    t_chave chaves[M-1]; /* Estrutura de vetor do tipo t_chave, 4 */
    struct t_no *pFilhos[M]; /* Qtd  max de filhos 5*/
}t_no;

typedef struct root{
    t_no *root;
}t_root;

t_no *cria_no(){
    int i;
    t_no *no = (t_no*)malloc(sizeof(t_no));
    
    no->contador = 0;
    for (i=0;i<M-2;i++) {
     no->chaves[i].ident = -1;
      no->pFilhos[i] = NULL;
    }
    no->pFilhos[i] = NULL;
    
    return no;
}

t_root *criaArvore(){
    t_root *root = (t_root*)malloc(sizeof(t_root));
    
    root->root = cria_no();
    
    return root;
}

t_chave *criaChave(int ident, char chave[], int estPrr){
    t_chave *estChave = (t_chave*)malloc(sizeof(t_chave));
    
    estChave->ident = ident;
    strcpy(estChave->chave, chave);
    estChave->prr = estPrr;
    
    return estChave;
}

int cliParser(int argc, char *argv[], int *registerType){
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
                    validate = 1;
                    *registerType = 1;
                } else if(strcmp(argv[3],"2") == 0){
                    printf("\nCarregando B-Tree de registros de tamanho fixo...\n\n");
                    validate = 1;
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

void insertBtree(t_chave *chave, t_root *root){
    int i;
    int spliter;
    t_no *raiz = root->root;
    t_no *novaRaiz;
    t_no *folha;
    
    if(raiz->contador != 4){ /* Checa se a raiz possui mesno de 4 chaves */
        i = 0;
        while(raiz->chaves[i].ident == -1){
            i++;
        }
        raiz->chaves[i] = *chave;
        raiz->contador += 1;
        printf("\tFoi\n");
    } else {
        spliter = raiz->contador + 1;
        spliter = ceil(spliter/2.0f);
        
        novaRaiz = cria_no();
        novaRaiz->chaves[0] = raiz->chaves[spliter-1];
        novaRaiz->contador += 1;
        
        folha = cria_no();
        folha->chaves[0] = raiz->chaves[spliter];
        folha->contador += 1;
         
        printf("\tSoon\n");
    }
}

void pegaChaveVariavel(t_root *root){
    char string[8];
    int i = 0;
    int j;
    int k = 1;
    t_chave *chave;
    
    while(!feof(fp)){
        j = 0;
        
        printf("%d\t", i);
        while(fgetc(fp) != ';'){
            fseek(fp, i, SEEK_SET);
            string[j] = fgetc(fp);
            i++;
            j++;
        }
        printf("%s\t%d", string, k);
        k++;
        chave = criaChave(k, string, i);
        insertBtree(chave, root);
        while((fgetc(fp) != '\n')&&(!feof(fp))){
            fseek(fp,i,SEEK_SET);
            i++;
        }
    }
}

void loadFixedBTree(){
    
}



void loadVariableBTree(){
    while(!feof(fp)){
        
    }

}

int main(int argc, char *argv[]){
    int registerType = 0;
    int cliCheck = cliParser(argc, argv, &registerType);
    t_root *root;

    if (cliCheck == 1){
        if (registerType == 1){
            root = criaArvore();
            pegaChaveVariavel(root);
            /*loadVariableBTree();*/
        } else if(registerType == 2) {
            loadFixedBTree();
        } else {
            printf("Erro inesperado!");
        }
    }


    return 0;
}
