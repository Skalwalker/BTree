#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define M 5
#define TRUE 1
#define FALSE 0
#define T 2

FILE *fp;

typedef struct chave{
    int ident;
    char chave[8];
    int prr; /* Qtd de prr, 4*/
}t_chave;

typedef struct no{
    int contador; /* Conta a contidade de chaves, 4 */
    int folha;
    t_chave chaves[M];//M-1 /* Estrutura de vetor do tipo t_chave, 4 */
    struct no *pFilhos[M+1];//M /* Qtd  max de filhos 5*/
}t_no;

// typedef struct root{
//     t_no *root;
// }t_root;

t_no *cria_no(){
    int i;
    t_no *no = (t_no*)malloc(sizeof(t_no));
    
    no->contador = 0;
    no->folha = FALSE;
    for (i=0;i<M-2;i++) {
        no->chaves[i].ident = -1;
        no->pFilhos[i] = NULL;
    }
    no->pFilhos[i] = NULL;
    
    return no;
}

t_no *criaArvore(){
    t_no *root = (t_no*)malloc(sizeof(t_no));
    
    root = cria_no();
    root->folha = TRUE;
    
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
void splitChild(t_no *nodeX, int i, t_no *newNodeY){
    int j;
    t_no *newNodeZ = (t_no*)malloc(sizeof(t_no));
    newNodeY = nodeX->pFilhos[i];
    
    newNodeZ->folha = newNodeY->folha;
    newNodeZ->contador = T-1;
    
    for(j = 1; j <= T-1; j++){
        newNodeZ->chaves[j] = newNodeY->chaves[j+T];
    }
    if(!newNodeY->folha){
        for(j = 1; j <= T; j++){
            newNodeZ->pFilhos[j] = newNodeY->pFilhos[j+T];
        }
    }
    newNodeY->contador = T-1;
    for(j = nodeX->contador+1; j >= i+1; j--){
        nodeX->chaves[j+1] = nodeX->chaves[j];
    }
    nodeX->pFilhos[i+1] = newNodeZ;
    for (j = nodeX->contador; j >= i; j--){
        nodeX->chaves[j+1] = nodeX->chaves[j];
    }
    nodeX->chaves[i] = newNodeY->chaves[T];
    nodeX->contador += 1;
}

void insertNonFull(t_no *node, t_chave *toInsert){
    int i = node->contador;
    
    if(node->folha){
        while(i >= 1 && ((toInsert->ident) < (node->chaves[i].ident))){
            node->chaves[i+1] = node->chaves[i];
            i -= 1; 
        }
        node->chaves[i+1] = *toInsert;
        node->contador += 1;
    } else {
        while(i >= 1 && ((toInsert->ident) < (node->chaves[i].ident))){
            i -= 1;
        }
        i += 1;
        if(node->pFilhos[i]->contador == 4){
            splitChild(node, i, node->pFilhos[i]); // Colocar terceiro parametro;
            if((toInsert->ident) > (node->chaves[i].ident)){
                i += 1;
            }
        }
        insertNonFull(node->pFilhos[i], toInsert);
    }
}


void insertBtree(t_no *root, t_chave *chave){
    t_no *newNode;
    t_no *rootAux;
    
    rootAux = root; 
    if(root->contador == 4){
        /* Caso o no esteja cheio temos que executar o split*/
        newNode = (t_no*)malloc(sizeof(t_no));
        root = newNode;
        newNode->folha = FALSE;
        newNode->contador = 0;
        newNode->pFilhos[1] = rootAux;
        splitChild(newNode, 1, rootAux);
        insertNonFull(newNode, chave);
    } else {
        insertNonFull(rootAux, chave);
    }
}

void pegaChaveVariavel(t_no *root){
    char string[8];
    int i = 0;
    int j;
    int k = 0;
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
        insertBtree(root, chave);
        printf("\tInseriu >%d< \n", k);
        if(k == 5) break;
        while((fgetc(fp) != '\n')&&(!feof(fp))){
            fseek(fp,i,SEEK_SET);
            i++;
        }
    }
}

void printBtree(t_no *root){
    int i;
    if(root == NULL){
        return;
    }
    for(i = 1; i <= root->contador; i++){
        printBtree(root->pFilhos[i]);
        printf("%d ", root->chaves[i].ident);
    }
    printBtree(root->pFilhos[root->contador]);
}

int main(int argc, char *argv[]){
    int registerType = 0;
    int cliCheck = cliParser(argc, argv, &registerType);
    t_no *root;
    
    int i = 0;

    if (cliCheck == 1){
        if (registerType == 1){
            root = criaArvore();
            pegaChaveVariavel(root);
            printf("\n");
            for(i = 1; i <= 4; i++){
                printf("%d\n", root->chaves[i].ident);
            }
            printBtree(root);
        } else if(registerType == 2) {
        } else {
            printf("Erro inesperado!");
        }
    }


    return 0;
}
