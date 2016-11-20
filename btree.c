#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define M 5
#define TRUE 1
#define FALSE 0
#define T 3
#define NOME 69
#define DATA 22
#define EMPRESA 26

FILE *fp;
int contNos = 0;

/* Structure of a key */
typedef struct chave{
    int ident; /* Identifier of the key */
    char chave[8]; /* Key as string size */
    int prr; /* PRR Value on file */
}t_chave;

/* Structure of a node */
typedef struct no{
    int ident;
    int contador; /* Counter */
    int folha; /* Tell if the node is a leaf */
    t_chave chaves[M];//M-1 /* Vector of keys */
    struct no *pFilhos[M+1];//M /* Vector of nodes */
}t_no;

typedef struct Tree{
    t_no *root;
}t_tree;

/* Create the node */
t_no *cria_no(int newIdent){
    int i;
    /* Alloc memory*/
    t_no *no = (t_no*)malloc(sizeof(t_no));
    
    no->ident = newIdent;
    contNos++;
    /* Set counter to 0*/
    no->contador = 0;
    /* Set the leaf indicator to false*/
    no->folha = FALSE;
    /* Populate the vector of keys and son with default values */
    for (i=1 ; i <= M-1; i++) {
        /* -1 is set to a non used key */
        no->chaves[i].ident = 0;
        /* NULL is set if the son have no pointer */
        no->pFilhos[i] = NULL;
    }
    /* Extra NULL set because there is one more pointer to son */
    no->pFilhos[i] = NULL;
    
    /* Return the node */
    return no;
}

/* Create a new tree */
t_tree *criaArvore(t_tree *tree){
    /* Create the root node */
    t_no *x = cria_no(contNos);
    /* Set as a leaf to true */ 
    x->folha = TRUE;
    /* Set the new root as the tree root */
    tree->root = x;
    /* Return the new root */
    return tree;
}


/* Method for populating the key */
t_chave *criaChave(int ident, char chave[], int estPrr){
    /* Alloc space for the new struct key */
    t_chave *newKey = (t_chave*)malloc(sizeof(t_chave));
    /* Set the stuct key identifier */
    newKey->ident = ident;
    /* Set the struct key key */
    strcpy(newKey->chave, chave);
    /* Set the struct key prr */
    newKey->prr = estPrr;
    /* Return the new key */
    return newKey;
}

/* Method for handling the Command Line Arguments */
int cliParser(int argc, char *argv[], int *registerType){
    int validate = 0;
    
    /* Check the amount of parsed args */
    if (argc != 4){
        printf("Uso: %s <nome_arquivo> -r 1 ou 2\n\n1 - Indica Registros de tamanho variavel\n2 - Indica Registros de tamanho fixo\n\n", argv[0]);
        
        validate = 0;
    } else{
        fp = fopen(argv[1], "r");

        /* Check if the file passed as arg really exist. */
        if (fp == NULL){
            printf("\nErro ao abrir arquivo, encerrando...\n\n");
            validate = 0;
        } else {
            /* Check if the arg is right */
            if(strcmp(argv[2],"-r")){
                printf("\nErro! Parametro nao reconhecido, voce quis dizer '-r' ?\n\n");
                validate = 0;
            } else {
                /* Check if the value for the arg is valid and reference the value as an int */
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
    
    /* Return if the CLI is ok*/
    return validate;
}


void splitChild(t_no *nodeX, int i, t_no *newNodeY){
    int j;
    t_no *newNodeZ = cria_no(contNos);
    newNodeY = nodeX->pFilhos[i];
    
    newNodeZ->folha = newNodeY->folha;
    newNodeZ->contador = 2-1;
    
    for(j = 1; j <= T; j++){
        newNodeZ->chaves[j] = newNodeY->chaves[j+T];
    }
    if(newNodeY->folha == FALSE){
        for(j = 1; j <= T; j++){
            newNodeZ->pFilhos[j] = newNodeY->pFilhos[j+T];
        }
    }
    newNodeY->contador = T-1;
        
    for(j = nodeX->contador+1; j >= i+1; j--){
        nodeX->pFilhos[j+1] = nodeX->pFilhos[j];
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
    
    if(node->folha == TRUE){
        while((i >= 1) && ((toInsert->ident) < (node->chaves[i].ident))){
            node->chaves[i+1] = node->chaves[i];
            i -= 1; 
        }
        node->chaves[i+1] = *toInsert;
        node->contador += 1;
        
    } else {
        while((i >= 1) && ((toInsert->ident) < (node->chaves[i].ident))){
            i -= 1;
        }
        i += 1;
        if(node->pFilhos[i]->contador == 4){
            splitChild(node, i, node->pFilhos[i]);
            if((toInsert->ident) > (node->chaves[i].ident)){
                i += 1;
            }
        }
        insertNonFull(node->pFilhos[i], toInsert);
    }
}


void insertBtree(t_tree *tree, t_chave *chave){
    t_no *newNode;
    t_no *newRoot = tree->root;
    t_no *aux;

    if(newRoot->contador == 4){
        /* Caso o no esteja cheio temos que executar o split*/
        newNode = (t_no*)malloc(sizeof(t_no));
        tree->root = newNode;
        newNode->folha = FALSE;
        newNode->contador = 0;
        newNode->pFilhos[1] = newRoot;
        splitChild(newNode, 1, newRoot);
        insertNonFull(newNode, chave);
    } else {
        insertNonFull(newRoot, chave);
    }
}

void pegaChave(t_tree *root, int tamReg){
    char string[tamReg];
    int i = 0;
    int j = 0;
    int k = 0;
    t_chave *chave;
    
    while(!feof(fp)){
        j = 0;
        
        printf("%d\t", i);
        if(tamReg == 8){
            while(fgetc(fp) != ';'){
                fseek(fp, i, SEEK_SET);
                string[j] = fgetc(fp);
                i++;
                j++;
            }
        } else {
            while(j < 5){
                fseek(fp, i, SEEK_SET);
                string[j] = fgetc(fp);
                i++;
                j++;
            }
        }
        k++;
        printf("%s\t%d", string, k);
        chave = criaChave(k, string, i);
        insertBtree(root, chave);
        printf("\tInseriu >%d< \n", k);
        while((fgetc(fp) != '\n')&&(!feof(fp))){
            fseek(fp,i,SEEK_SET);
            i++;
        }
    }
}


void printBtree (t_no *a, int level) {
   int i;
   int j = 0;
   int k = 0;


   for (i = 0; i < level; i++) { printf("  "); }
   // printf("Contador de a === %d\n", a->contador);
   printf("|");
   for (i = 1; i <= a->contador; i++) {
      printf("%d|", a->chaves[i].ident);
   }
   printf("\n");
   
   while(k <= 5){
       if(a->pFilhos[k] != NULL) j++;
       k++;
   }

   for (i = 1; i <= j; i++) {
      if (a->folha == FALSE) {
         printBtree(a->pFilhos[i], level + 1);
     }
   }
}

// void printBtree(t_no *node, int level){
//     int i = 0;
//     
//     printf("N%d\t", level);
//     for(i = 1; i <= 5; i++){
//         if(node->pFilhos[i] != NULL){
//             printf("P%d\n", );
//         }
//     }
// }

void inserirRegistroVariavel(){
    
}

void inserirRegistroFixo(){}

t_no *searchBTree(t_no *root, int k, int *retI, int *seeks){
    int i = 1;
    while((i<=root->contador) && (k > root->chaves[i].ident)){
        i += 1;
    }
    if((i<=root->contador) && (k == root->chaves[i].ident)){
        *retI = i;
        return root;
    } else if(root->folha){
        *seeks++;
        return NULL;
    } else {
        return searchBTree(root->pFilhos[i], k, retI, seeks);
    }
    
}

int menuopc(){
    int option = 0;
            
    while(option != 1 || option != 2 || option != 3){
        
        printf("\n1 - Buscar Registro\n2 - Inserir Registro\n3 - Mostrar arvore-B\n\n>>>");
        scanf("%d", &option);
        
        if(option == 1){
            return 1;
        } else if(option == 2){
            return 2;
        } else {
            return 3;
        }
    }
    
    
}


int main(int argc, char *argv[]){
    int option, prrAux, chaveInt, seeks = 0, retI= 0, registerType = 0;;
    int i = 0;
    int j = 0;
    char chavePes[10];
    char stringOut[200];
    t_tree *root = (t_tree*)malloc(sizeof(t_tree));
    t_no *pesquisa = (t_no*)malloc(sizeof(t_no));
    
    int cliCheck = cliParser(argc, argv, &registerType);

    if (cliCheck == 1){
            
        root = criaArvore(root);
        if(registerType == 1){
            pegaChave(root, 8);
            
        } else {
            pegaChave(root, 5);
        }

        option = menuopc();
        
        if(option == 1){
            printf("Insira a chave primaria a ser pesquisada: ");
            scanf("%s", chavePes);
            getchar();
            if(registerType == 1){
                for(i = 0; i < 6; i++){
                    chavePes[i] = chavePes[i+2];
                }
                chavePes[5] = '\0';
            } else {
                chavePes[0] = chavePes[2];
                chavePes[1] = chavePes[3];
                chavePes[2] = '\0';
            }            
            
            chaveInt = atoi(chavePes);
            pesquisa = searchBTree(root->root, chaveInt, &retI, &seeks);
            rewind(fp);
            
            prrAux = pesquisa->chaves[retI].prr;
            j = 0;
            fseek(fp, prrAux, SEEK_SET);
            if(registerType == 1){
                prrAux++;

                while((fgetc(fp) != '\n') && (!feof(fp))){
                    while((fgetc(fp) != ';') && (fgetc(fp) != '\n')){
                        fseek(fp, prrAux, SEEK_SET);
                        stringOut[j] = fgetc(fp);
                        prrAux++;
                        j++;
                    }
                    stringOut[j] = '\0';
                    printf("%s\n", stringOut);
                    j = 0;
                    prrAux++;
                    fseek(fp, prrAux, SEEK_SET);
                }
            } else {
                int contador = 0;
                int contadorAll = 0;
                int selection = 0;
                int flag = 0;
                int tam = NOME;
                i = 0;
                rewind(fp);
                while(contadorAll < 123 || selection < 3){
                    while(contador < tam && flag != 2){
                        printf("%d\n", contador);
                        fseek(fp, prrAux, SEEK_SET);
                        stringOut[i] = fgetc(fp);
                        printf("%s\n", stringOut);
                        
                        if(fgetc(fp) == ' '){
                            flag++;
                        } else {
                            flag = 0;
                        }
                        prrAux++;
                        getchar();
                        contador++;
                    }
                    if(stringOut[i] == ' ' && stringOut[i+1] == ' '){
                        stringOut[i] = '\0';
                    }
                    selection++;
                    if (selection == 1){
                        tam = DATA;
                    } else {
                        tam = EMPRESA;
                    }
                    printf("%s\n", stringOut);
                }
            }
            
        } else if(option == 2){
            
        } else {
            // printBtree(root->root, 0);
        }

        // printBtree(root->root, 0);
        //     pesquisa = searchBTree(root->root, i, &retI);
        //     printf("%d ", pesquisa->chaves[retI].ident);

    }


    return 0;
}
