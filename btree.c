#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define M 5
#define TRUE 1
#define FALSE 0
#define T 3

FILE *fp;

/* Structure of a key */
typedef struct chave{
    int ident; /* Identifier of the key */
    char chave[8]; /* Key as string size */
    int prr; /* PRR Value on file */
}t_chave;

/* Structure of a node */
typedef struct no{
    int contador; /* Counter */
    int folha; /* Tell if the node is a leaf */
    t_chave chaves[M];//M-1 /* Vector of keys */
    struct no *pFilhos[M+1];//M /* Vector of nodes */
}t_no;

typedef struct Tree{
    t_no *root;
}t_tree;

/* Create the node */
t_no *cria_no(){
    int i;
    /* Alloc memory*/
    t_no *no = (t_no*)malloc(sizeof(t_no));
    
    /* Set counter to 0*/
    no->contador = 0;
    /* Set the leaf indicator to false*/
    no->folha = FALSE;
    /* Populate the vector of keys and son with default values */
    for (i=0;i<M-2;i++) {
        /* -1 is set to a non used key */
        no->chaves[i].ident = -1;
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
    t_no *x = cria_no();
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
        printf("Uso: %s nome_arquivo -r 1 ou 2\n\n1 - Indica Registros de tamanho variavel\n2 - Indica Registros de tamanho fixo\n\n", argv[0]);
        
        validate = 0;
    } else{
        fp = fopen(argv[1], "r" );

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
        if(node->pFilhos[i]->contador == 2*T-1){
            splitChild(node, i, node->pFilhos[i]); // Colocar terceiro parametro;
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

    if(newRoot->contador == 2*T-1){
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

void pegaChaveVariavel(t_tree *root){
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
        while((fgetc(fp) != '\n')&&(!feof(fp))){
            fseek(fp,i,SEEK_SET);
            i++;
        }
    }
}


void printBtree (t_no *a, int level) {
   int i;


   for (i = 0; i < level; i++) { printf("  "); }


   printf("|");
   for (i = 1; i <= a->contador; i++) {
      printf("%d|", a->chaves[i].ident);

   }
   printf("\n");


   for (i = 1; i <= a->contador; i++) {
      if (a->folha == FALSE) {
         printBtree(a->pFilhos[i], level + 1);
      }
   }
}

t_no *searchBTree(t_no *root, int k, int *retI){
    int i = 1;
    
    while((i<=root->contador) && (k > root->chaves[i].ident)){
        i += 1;
    }
    if((i<=root->contador) && (k == root->chaves[i].ident)){
        *retI = i;
        return root;
    } else if(root->folha){
        return NULL;
    } else {
        return searchBTree(root->pFilhos[i], k, retI);
    }
    
}

// void printBtree(t_no *node){
//     int i;
//     if(node == NULL){
//         return;
//     }
//     for(i = 1; i <= node->contador; i++){
//         printBtree(node->pFilhos[i]);
//         printf("%d ", node->chaves[i].ident);
//     }
//     printBtree(node->pFilhos[node->contador]);
// }

int main(int argc, char *argv[]){
    int registerType = 0;
    int retI= 0;
    int cliCheck = cliParser(argc, argv, &registerType);
    t_tree *root = (t_tree*)malloc(sizeof(t_tree));
    t_no *pesquisa = (t_no*)malloc(sizeof(t_no));

    if (cliCheck == 1){
        if (registerType == 1){
            root = criaArvore(root);
            pegaChaveVariavel(root);
            printf("\n");
            //printBtree(root->root, 5);
            pesquisa = searchBTree(root->root, 1, &retI);
            printf("%d", pesquisa->chaves[retI].ident);
            
        } else if(registerType == 2) {
        } else {
            printf("Erro inesperado!");
        }
    }


    return 0;
}
