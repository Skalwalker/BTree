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
int last = 0;

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

/* Structure of a Tree */
typedef struct Tree{
    t_no *root;
}t_tree;


typedef struct Elemento{
    t_no *no;
    int level;
    int pagina;
    struct Elemento *proximo;
}t_elemento;
/* Structure of a list to implement a queue for print */
typedef struct lista{
	t_elemento *primeiro;
	t_elemento *ultimo;
}t_lista;

/* Structure of a queue */
typedef struct fila{
	t_lista *l;
}t_fila;


/*Start of list functions*/
t_lista *criaLista(){
   t_lista *l = (t_lista *)malloc(sizeof(t_lista));
   l->primeiro = NULL;
   l->ultimo = NULL;
   return l;
}

void insereFinal(t_no *no, int level, int pagina, t_lista *l){
   t_elemento *novoultimo = (t_elemento*)malloc(sizeof(t_elemento));
   novoultimo->no = no;
   novoultimo->level = level;
   novoultimo->pagina = pagina;
   novoultimo->proximo = NULL; 
   if(l->primeiro == NULL){
      l->primeiro = novoultimo;
   }else{
     l->ultimo->proximo = novoultimo;
   }
   l->ultimo = novoultimo;
}

int estaVazia(t_lista *l){
    if(l->primeiro == NULL){
       return 1;
    }
    return 0;
}

t_elemento *removeInicio(t_lista *l){
    if(estaVazia(l)){
        return NULL;
    }
    t_elemento *tmp = l->primeiro;
    t_elemento *removido = l->primeiro;
    l->primeiro = l->primeiro->proximo;
    free(removido);
    if(l->primeiro == NULL){
       l->ultimo = NULL;
    }
    return tmp;
}
/*End of list functions*/


/* Start of Queue Functions */
t_fila *criaFila(){
   t_fila *f =  (t_fila*)malloc(sizeof(t_fila));
   f->l = criaLista();
   return f;
}

void enfileirar(t_no *no, int level, int pagina, t_fila *f){
  insereFinal(no, level, pagina, f->l);
}

t_elemento *desenfileirar(t_fila *f){
  return removeInicio(f->l);
}

int estaVaziaFila(t_fila *f){
    return estaVazia(f->l);
}
/* End of Queue Functions */


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
            while(j < 4){
                fseek(fp, i, SEEK_SET);
                string[j] = fgetc(fp);
                i++;
                j++;
            }
        }
        k++;
        last = k;
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


// void printBtree (t_no *a, int level) {
//    int i;
//    int j = 0;
//    int k = 0;
// 
// 
//    for (i = 0; i < level; i++) { printf("  "); }
//    printf("|");
//    for (i = 1; i <= a->contador; i++) {
//       printf("%d|", a->chaves[i].ident);
//    }
//    printf("\n");
// 
//    while(k <= 5){
//        if(a->pFilhos[k] != NULL) j++;
//        k++;
//    }
// 
//    for (i = 1; i <= j; i++) {
//       if (a->folha == FALSE) {
//          printBtree(a->pFilhos[i], level + 1);
//      }
//    }
// }

t_no *searchBTree(t_no *root, int k, int *retI, int *seeks){
    int i = 1;
    while((i<=root->contador) && (k > root->chaves[i].ident)){
        i += 1;
    }
    if((i<=root->contador) && (k == root->chaves[i].ident)){
        *retI = i;
        return root;
    } else if(root->folha){
        *seeks = *seeks + 1;
        return NULL;
    } else {
        return searchBTree(root->pFilhos[i], k, retI, seeks);
    }

}

void insertFile(char *argv[], t_tree *root ,int *registerType){
    char aux[8], aux2[8];
    char nome[100],  sobrenome[100], empresa[100], endereco[100];
    char cidade[100],  estado[3], zip[100], cell1[100], cell2[100];
    int file_pos, w = 0;
    
    char nomeJogo[NOME], data[DATA], produtora[EMPRESA], aux3[123];
    t_chave *chave;
    
    if(*registerType == 1){
        
        printf("Vamos inserir um novo registro:\nNome: ");
        getchar();
        scanf("%[^\n]%*c", nome);
        printf("\nSobrenome: ");
        scanf("%[^\n]%*c", sobrenome);
        printf("\nEmpresa: ");
        scanf("%[^\n]%*c", empresa);
        printf("\nEndereco: ");
        scanf("%[^\n]%*c", endereco);
        printf("\nCidade: ");
        scanf("%[^\n]%*c", cidade);
        printf("\nEstado (2 CHAR!): ");
        scanf("%[^\n]%*c", estado);
        printf("\nZIP: ");
        scanf("%[^\n]%*c", zip);
        printf("\nTelefone 1: ");
        scanf("%[^\n]%*c", cell1);
        printf("\nTelefone 2: ");
        scanf("%[^\n]%*c", cell2);
        
        if (last+1 < 10) {
            for(w=0;w<6;w++){
                if (w<2){
                    aux[w] = estado[w];
                } else {
                    aux[w] = '0';
                }
            }
            aux[w] = '\0';
            sprintf(aux2,"%d",last+1);
            strcat(aux, aux2);
        } else if(last+1 < 100){
            for(w=0;w<5;w++){
                if (w<2){
                    aux[w] = estado[w];
                } else {
                    aux[w] = '0';
                }
            }
            aux[w] = '\0';
            sprintf(aux2,"%d",last+1);
            strcat(aux, aux2);
        }else if(last + 1 < 1000){
            for(w=0;w<4;w++){
                if (w<2){
                   aux[w] = estado[w];
               } else {
                   aux[w] = '0';
               }
            }
            aux[w] = '\0';
            sprintf(aux2,"%d",last+1);
            strcat(aux, aux2);
        }else if(last + 1 < 10000){
            for(w=0;w<3;w++){
                if (w<2){
                    aux[w] = estado[w];
                } else {
                    aux[w] = '0';
                }
            }
            aux[w] = '\0';
            sprintf(aux2,"%d",last+1);
            strcat(aux, aux2);
        } else if(last + 1 <= 99999){
            for(w=0;w<2;w++){
                if (w<2){
                    aux[w] = estado[w];
                } else {
                    aux[w] = '0';
                }
            }
            aux[w] = '\0';
            sprintf(aux2,"%d",last+1);
            strcat(aux, aux2);
        }

        fprintf(fp, "\r\n%s;", aux);
        file_pos = ftell(fp);
        printf("\n\nChave dos dados inseridos: %s\n", aux);
        fprintf(fp, "%s;", nome);
        fprintf(fp, "%s;", sobrenome);
        fprintf(fp, "%s;", empresa);
        fprintf(fp, "%s;", endereco);
        fprintf(fp, "%s;", cidade);
        fprintf(fp, "%s;", estado);
        fprintf(fp, "%s;", zip);
        fprintf(fp, "%s;", cell1);
        fprintf(fp, "%s", cell2);

        chave = criaChave(last+1, aux, file_pos-1);
        insertBtree(root, chave);
        
        last = last + 1;
        
    } else {
        char id[5];
        char aux4[2];
        printf("Vamos inserir um novo registro: ");
        getchar();
        printf("\nNome do Jogo: ");
        scanf("%[^\n]%*c", nomeJogo);
        printf("\nData: ");
        scanf("%[^\n]%*c", data);
        printf("\nProdutora: ");
        scanf("%[^\n]%*c", produtora);
        
        id[0] = 'I';
        id[1] = 'D';
        id[2] = '\0';
        sprintf(aux4,"%d",last+1);
        strcat(id, aux4);
        int i, j = 0, k = 0, l = 0;
        for(i=0;i<122;i++){
            if(i<5){
                if(i<4){
                    aux3[i] = id[i] ;
                } else{
                    aux3[i] = ' ';
                }
            } else if(i<74){
                if(nomeJogo[j] == '\0'){
                    aux3[i] = ' ';
                } else {
                    aux3[i] = nomeJogo[j];
                    j++;
                }
            } else if(i<96){
                if(data[k] == '\0'){
                    aux3[i] = ' ';
                } else {
                    aux3[i] = data[k];
                    k++;
                }
            } else if(i<121){
                if(produtora[l] == '\0'){
                    aux3[i] = ' ';
                } else {
                    aux3[i] = produtora[l];
                    l++;
                }
            }
            
        }
        
        aux3[i] = '\0';
        fprintf(fp, "\r\n");
        file_pos = ftell(fp);
        fprintf(fp, "%s", aux3);
        printf("ID de chave inserida: %s", id);
        
        chave = criaChave(last+1, id, file_pos+5);
        insertBtree(root, chave);
        
        last = last + 1;
        
    }
}

void searchFile(char *argv[], t_tree *root ,int *registerType){
    t_no *pesquisa = (t_no*)malloc(sizeof(t_no));
    char chavePes[10];
    char stringOut[200];
    int i = 0, j = 0, contador = 0, contadorAll = 0, selection = 0, flag = 0;
    int prrAux, chaveInt, seeks = 0, retI= 0;
    int tam = NOME;
    
    printf("Insira a chave primaria a ser pesquisada: ");
    scanf("%s", chavePes);
    getchar();
    printf("\n");
    if(*registerType == 1){
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
    if(*registerType == 1){
        prrAux++;

        while((fgetc(fp) != '\n') && (!feof(fp))){
            while((fgetc(fp) != ';') && (fgetc(fp) != '\n') && (!feof(fp))){
                fseek(fp, prrAux, SEEK_SET);
                stringOut[j] = fgetc(fp);
                prrAux++;
                j++;
            }
            stringOut[j] = '\0';
            prrAux++;
            fseek(fp, prrAux, SEEK_SET);
            j = 0;
            
            printf("|%s\n", stringOut);
        }
    } else {
        i = 0;
        rewind(fp);
        
        while(contadorAll < 123 && selection < 3){
            while(contador < tam && flag != 2){
                fseek(fp, prrAux, SEEK_SET);
                stringOut[i] = fgetc(fp);

                if(fgetc(fp) == ' '){
                    flag++;
                } else {
                    flag = 0;
                }
                prrAux++;
                contador++;
                contadorAll++;
                i++;
            }
            
            
            if(flag == 2){
                stringOut[i-1] = '\0';
            } else {
                stringOut[i] = '\0';
            }
            
            prrAux += (tam - contador);
            selection++;
            
            if (selection == 1){
                tam = DATA;
            } else {
                tam = EMPRESA;
            }
            
            i = 0;
            contador = 0;
            flag = 0;
            
            printf("|%s\n", stringOut);
        }
    }
}


void printBtree(t_no* root){
    t_fila *fila;
    int i = 0, j = 0, k = 0, w = 1;
    int pagina = 0;
    int level = 0;
    t_no *no;
    t_elemento *elemento = (t_elemento*)malloc(sizeof(t_elemento));
    
    fila = criaFila();
    enfileirar(root, level, pagina, fila);
    
    while(!estaVaziaFila(fila)){
        elemento = desenfileirar(fila);
        no = elemento->no;
        if((elemento->level != level)||(level == 0)){
            printf("\n\nN%d:\tP%d:\t", elemento->level, elemento->pagina);
        } else {
            printf(";\t\tP%d:\t", elemento->pagina);
        }
        
        for(i=1;i<=no->contador;i++){
            printf("%s", no->chaves[i].chave);
            if(i != no->contador){
                printf("|");
            }
        }
        
        while(k <= 5){
            if(no->pFilhos[k] != NULL){
                j++;
            } 
            k++;
        }
        
        printf("\t");
        for(i = 1; i<=no->contador+1; i++){
            if(no->folha == FALSE){
                printf("%d", elemento->pagina+w);
                w++;
                if(i!=no->contador+1){
                    printf("|");
                }
            } else {
                printf("-");
                if(i!=no->contador+1){
                    printf("|");
                }
            }
            
        }
        w--;

        level = elemento->level;
        

        for (i = 1; i <= no->contador+1; i++) {
            if(no->pFilhos[i] != NULL){
                pagina++;
                enfileirar(no->pFilhos[i], level + 1, pagina, fila);
            }
        }
        
    }

} 


int menuopc(){
    int option = 0;

    while(option != 1 && option != 2 && option != 3 && option != 4){

        printf("\n1 - Buscar Registro\n2 - Inserir Registro\n3 - Mostrar arvore-B\n4 - Finalizar Programa\n\n>>>");
        scanf("%d", &option);
        
    }
    
    return option;
}

int main(int argc, char *argv[]){
    int option = 0, registerType = 0;
    t_tree *root = (t_tree*)malloc(sizeof(t_tree));
    int cliCheck = cliParser(argc, argv, &registerType);


    if (cliCheck == 1){
        root = criaArvore(root);
        if(registerType == 1){
            pegaChave(root, 8);
        } else {
            pegaChave(root, 5);
        }
        fclose(fp);
        while(option != 4){
            
            option = menuopc();
            
            if(option == 1){
                fp = fopen(argv[1], "r");
                searchFile(argv, root, &registerType);
                fclose(fp);
            } else if(option == 2){
                fp = fopen(argv[1], "a");
                insertFile(argv, root, &registerType);
                fclose(fp);
            } else if(option == 3) {
                printBtree(root->root);
                printf("\n");
            }
        }
    }


    return 0;
}
