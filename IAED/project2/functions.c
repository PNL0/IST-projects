/* 
* Ficheiro: funcoes.c

* Autor: Pedro Nunes Leal (ist1106154)

* Descrição: Este programa tem as funções que foram usadas no projeto.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "project2.h"

/* Limpa a memória dos vetores das paragens,carreiras e ligações do sistema. */
void limpa_vetores(Paragem *paragens, int num_paragens, Carreira *carreiras,
                    int num_carreiras, Ligacao *ligacoes){
    int i;
    for(i = 0; i < num_paragens; i++)
        free(paragens[i].nome);
    free(paragens);
    for(i = 0; i < num_carreiras; i++)
        free(carreiras[i].nome);
    free(carreiras);
    free(ligacoes);
}

/* Verifica se a memória está esgotada. */
void verifica_memoria(Paragem *paragens, int num_paragens, Carreira *carreiras,
                    int num_carreiras, Ligacao *ligacoes, void *ponteiro){
    if(ponteiro == NULL){
        limpa_vetores(paragens,num_paragens,carreiras,num_carreiras,ligacoes);
        printf("No memory.");
        exit(0);
    }
}

/* Conta o número de argumentos de uma linha. */
int conta_argumentos(char linha [LINHA_MAX]){
    int estado = FORA, i, tamanho, contador = 0;
    tamanho = strlen(linha);
    for (i = 0; i < tamanho; i++){
        /* A funcao conta um argumento sempre que o estado muda de FORA
        para DENTRO ou de ESPECIAL para FORA. */
        switch (estado){
            case FORA:
                if(linha[i] == '"')
                    estado = ESPECIAL;
                else if(linha[i]!= ' ' && linha[i]!= '\n' && linha[i]!= '\t'){
                    estado = DENTRO;
                    contador++;
                }
                break;
            case DENTRO:
                if(linha[i] == ' ')
                    estado = FORA;
                break;
            case ESPECIAL:
                if(linha[i] == '"'){
                    estado = FORA;
                    contador++;
                }
                break;
        }   
    }
    return contador;
} 

/* Verifica se uma dada carreira existe. */
int verifica_carreira(char linha[LINHA_MAX],int num_carreiras,int num_paragens,
                     Carreira *carreiras, Paragem *paragens,Ligacao *ligacoes){
    char *nome_carreira = NULL;
    int i;

    nome_carreira = (char*)malloc(sizeof(char) * strlen(linha));
    verifica_memoria(paragens, num_paragens,carreiras, num_carreiras,ligacoes,
                    nome_carreira);
    strcpy(nome_carreira, strtok(linha, " \n\t"));

    for(i = 0; i < num_carreiras; i++)
        if(carreiras[i].nome != NULL)
            if(strcmp(nome_carreira, carreiras[i].nome) == 0){
                free(nome_carreira);
                return 1;
            }
    free(nome_carreira);
    return 0;
}

/* Lista todas as carreiras do sistema. */
void lista_carreiras(Carreira *carreiras, int num_carreiras, Ligacao *ligacoes,
                    int num_ligacoes){
    int i, j, contador = 0, num_paragens;
    double custo = 0.00, duracao = 0.00;
    /* Para cada carreira é obtido o custo total, a duração total
    e o numero de paragens. */
    for(i = 0; i < num_carreiras; i++){
        for(j = 0; j < num_ligacoes; j++)
            if(carreiras[i].nome != NULL && ligacoes[j].carreira.nome != NULL)
                if(strcmp(carreiras[i].nome, ligacoes[j].carreira.nome) == 0){
                    custo += ligacoes[j].custo;
                    duracao += ligacoes[j].duracao;
                    contador++;
                }
        num_paragens = contador + 1;
        /* O contador ser 0 implica que a carreira nao tem nenhuma ligacao,
        logo nao vai ter paragem de origem ou destino. */
        if(contador == 0)
            printf("%s 0 %.2f %.2f\n",carreiras[i].nome, custo, duracao);
        else {
            printf("%s ", carreiras[i].nome);
            printf("%s ", carreiras[i].origem.nome);
            printf("%s ", carreiras[i].destino.nome);
            printf("%d %.2f %.2f\n", num_paragens, custo, duracao);
        }
        custo = 0.00;
        duracao = 0.00;
        contador = 0;
    }
}

/* Verifica a ordem pela qual as ligações de uma carreira estão no vetor. */
int verifica_ordem(Ligacao *ligacoes, int num_ligacoes, char *nome_carreira,
                char *origem, char *destino){
    int i;
    for(i = 0; i < num_ligacoes; i++)
        if(strcmp(nome_carreira, ligacoes[i].carreira.nome) == 0){
            if(strcmp(ligacoes[i].origem.nome, origem) == 0)
                /* As ligações aparecem pela ordem normal do percurso 
                da carreira. */
                return 0; 
            else if(strcmp(ligacoes[i].destino.nome, destino) == 0)
                /* As ligações aparecem pela ordem invertida do percurso 
                da carreira. */
                return 1; 
            break;
        }
    return 0;
}

/* Imprime paragens que sejam consecutivas do percuso de uma carreira. */
int imprime_paragens_1(Ligacao *ligacoes, char *nome_carreira, char *origem, 
                     char *destino, int i, int contador){

    if(strcmp(nome_carreira, ligacoes[i].carreira.nome) == 0)
        if(strcmp(origem, ligacoes[i].origem.nome) == 0){
            strcpy(origem, ligacoes[i].destino.nome);
            if(contador - 1){
                printf("%s, ",origem);
                contador--;
            }
            else 
                printf("%s\n", destino);
        }
    return contador;
}

/* Lista todas as paragens de uma carreira, desde a origem ate ao destino. */
void mostra_carreira(char linha[LINHA_MAX],Paragem *paragens, int num_paragens,
                    Carreira *carreiras, int num_carreiras, Ligacao *ligacoes, 
                    int num_ligacoes){
    char *nome_carreira = NULL, *origem = NULL, *destino = NULL;
    int i, flag, contador = 0;

    nome_carreira = (char*) malloc(sizeof(char) * strlen(linha));
    verifica_memoria(paragens, num_paragens, carreiras, num_carreiras, 
                    ligacoes, nome_carreira);
    strcpy(nome_carreira, strtok(linha, " \t\n"));
    /* Conta o número de ligações da carreira. */
    for(i = 0; i < num_ligacoes; i++)
        if(strcmp(nome_carreira, ligacoes[i].carreira.nome) == 0)
            contador++;
    /* Obtem a paragem de origem e de destino da carreira. */
    for(i = 0; i < num_carreiras; i++)
        if(strcmp(nome_carreira, carreiras[i].nome) == 0 && contador){ 
            origem = (char*) malloc(sizeof(char) * 
                            (strlen(carreiras[i].origem.nome) * 2));
            verifica_memoria(paragens, num_paragens, carreiras, num_carreiras,
                            ligacoes, origem);
            strcpy(origem, carreiras[i].origem.nome);
            destino = (char*) malloc(sizeof(char) * 
                            (strlen(carreiras[i].destino.nome) * 2));
            verifica_memoria(paragens, num_paragens, carreiras, num_carreiras,
                            ligacoes, destino);
            strcpy(destino, carreiras[i].destino.nome);
        }
    flag = verifica_ordem(ligacoes,num_ligacoes,nome_carreira,origem,destino);
    if(contador)
        printf("%s, ", origem);
    if(!(flag))
        for(i = 0; i < num_ligacoes; i++)
            contador = imprime_paragens_1(ligacoes, nome_carreira, origem,
                                        destino, i, contador);
    else 
        for(i = num_ligacoes-1; i >= 0; i--)
            contador = imprime_paragens_1(ligacoes, nome_carreira, origem,
                                        destino, i, contador);
    free(nome_carreira);
    free(origem);
    free(destino); 
}

/* Cria uma nova carreira. */
char* cria_carreira(char linha[LINHA_MAX], Paragem *paragens, int num_paragens,
                    Carreira *carreiras, int num_carreiras, Ligacao *ligacoes){

    carreiras[num_carreiras].nome =(char*)malloc(sizeof(char)*strlen(linha)+1);
    verifica_memoria(paragens, num_paragens, carreiras, num_carreiras, 
                    ligacoes, carreiras[num_carreiras].nome);
    strcpy(carreiras[num_carreiras].nome, strtok(linha, " \n\t"));
    return carreiras[num_carreiras].nome;
} 

/* Verifica se a string é um prefixo de tamanho pelo menos 3 da
palavra inverso. */
int verificaInverso(char s[]) {
    char inv[] = "inverso";
    int tamanho = strlen(s), i;

    if (tamanho < 3 || tamanho > 7)
        return 0;
    for (i = 0; i < tamanho; i++)
        if (inv[i] != s[i])
            return 0;
    return 1;
}

/* Imprime paragens de maneira inversa que sejam consecutivas do percuso 
de uma carreira. */
int imprime_paragens_2(Ligacao *ligacoes, char *nome_carreira, char *origem, 
                     char *destino, int i, int contador){

    if(strcmp(nome_carreira, ligacoes[i].carreira.nome) == 0)
        if(strcmp(destino, ligacoes[i].destino.nome) == 0){
            strcpy(destino, ligacoes[i].origem.nome);
            if(contador - 1){
                printf("%s, ",destino);
                contador--;
            }
            else 
                printf("%s\n", origem);
        }
    return contador;
}

/* Lista todas as paragens de uma carreira, desde o destino ate a origem. */
void mostra_carreira_inversa(char linha[LINHA_MAX], Paragem *paragens,
                        int num_paragens, Carreira *carreiras, 
                        int num_carreiras, Ligacao *ligacoes,int num_ligacoes){
    char *nome_carreira = NULL, *origem = NULL, *destino = NULL, *inv = NULL;
    int i, flag = 0, contador = 0;

    nome_carreira = (char*) malloc(sizeof(char) * strlen(linha));
    verifica_memoria(paragens, num_paragens, carreiras, num_carreiras,
                    ligacoes, nome_carreira);
    strcpy(nome_carreira, strtok(linha, " \t\n"));
    /* Obtem uma abreviatura da palavra "inverso". */
    inv = (char*) malloc(sizeof(char) * strlen(linha));
    verifica_memoria(paragens, num_paragens, carreiras, num_carreiras,
                    ligacoes, inv);
    strcpy(inv, strtok(NULL, " \n\t"));

    if(verificaInverso(inv)){
        /* Conta o número de ligações da carreira. */
        for(i = 0; i < num_ligacoes; i++)
            if(strcmp(nome_carreira, ligacoes[i].carreira.nome) == 0)
                contador++;
        /* Obtem a paragem de origem e de destino da carreira. */
        for(i = 0; i < num_carreiras; i++)
            if(strcmp(nome_carreira, carreiras[i].nome) == 0)
                if(contador){
                    origem = (char*) malloc(sizeof(char) * 
                            (strlen(carreiras[i].origem.nome) * 2));
                    verifica_memoria(paragens, num_paragens, carreiras, 
                                    num_carreiras,ligacoes, origem);
                    strcpy(origem, carreiras[i].origem.nome);
                    destino = (char*) malloc(sizeof(char) * 
                            (strlen(carreiras[i].destino.nome) * 2));
                    verifica_memoria(paragens, num_paragens, carreiras, 
                                    num_carreiras,ligacoes, destino);    
                    strcpy(destino, carreiras[i].destino.nome);
                }
        flag = verifica_ordem(ligacoes, num_ligacoes, nome_carreira, origem,
                             destino);
        if(contador)
            printf("%s, ", destino);
        if(!(flag))
            for(i = num_ligacoes-1; i >= 0; i--)
                contador = imprime_paragens_2(ligacoes, nome_carreira, 
                                            origem, destino, i, contador);
        else 
            for(i = 0; i < num_ligacoes; i++)
                contador = imprime_paragens_2(ligacoes, nome_carreira, 
                                            origem, destino, i, contador); 
    }
    else 
        printf("incorrect sort option.\n");
    free(nome_carreira);
    free(inv);
}

/* Obtem o indice de uma paragem. */
int obter_index_paragem(Paragem *paragens,int num_paragens,char *nome_paragem){
    int i;
    for (i = 0; i < num_paragens; i++)
        if (strcmp(paragens[i].nome, nome_paragem) == 0)
            return i;
    return -1;
}

/* Obtem o indice de uma carreira. */
int obter_index_carreira(Carreira *carreiras, int num_carreiras,
                        char *nome_carreira){
    int i;
    for (i = 0; i < num_carreiras; i++)
        if (strcmp(carreiras[i].nome, nome_carreira) == 0)
            return i;
    return -1;
}

/* Lista todas as paragens do sistema. */
void lista_paragens(Paragem *paragens, int num_paragens, Carreira *carreiras,
                   int num_carreiras, Ligacao *ligacoes, int num_ligacoes){                      
    int i, j, contador = 0, origem_index, destino_index, carreira_index;
    int **matriz = (int **)calloc(num_paragens, sizeof(int*));
    
    verifica_memoria(paragens, num_paragens, carreiras, num_carreiras, 
                    ligacoes, matriz);
    for (i = 0; i < num_paragens; i++){
        matriz[i] = (int *)calloc(num_carreiras, sizeof(int));
        verifica_memoria(paragens, num_paragens, carreiras, num_carreiras, 
                    ligacoes, matriz[i]);
    }
    for (i = 0; i < num_ligacoes; i++) {
        origem_index = obter_index_paragem(paragens, num_paragens, 
                                        ligacoes[i].origem.nome);
        destino_index = obter_index_paragem(paragens, num_paragens, 
                                        ligacoes[i].destino.nome);
        carreira_index = obter_index_carreira(carreiras, num_carreiras, 
                                        ligacoes[i].carreira.nome);
        /* Cada entrada da matriz tem uma paragem e uma carreira. Se a paragem
        pertence à carreira, então a entrada tem valor 1. Caso contrário tem
        valor 0. */
        matriz[origem_index][carreira_index] = 1;
        matriz[destino_index][carreira_index] = 1;
    }
    for (i = 0; i < num_paragens; i++){
        printf("%s: ",paragens[i].nome);
        printf("%16.12f ",paragens[i].latitude);
        printf("%16.12f",paragens[i].longitude);
        
        for(j = 0; j < num_carreiras; j++)
            if(matriz[i][j] == 1)
                contador++;
                
        printf(" %d\n", contador); 
        contador = 0;
    }
    for (i = 0; i < num_paragens; i++)
        free(matriz[i]);
    free(matriz);
}

/* Guarda o nome de uma paragem. */
void obtem_paragem(char linha[LINHA_MAX], char *nome_lido){
    int estado = FORA, i, j = 0, comprimento_linha;

    comprimento_linha = strlen(linha);
    for(i = 0; i < comprimento_linha; i++){
        switch (estado){
            case FORA:
                if(linha[i] == '"')
                    estado = ESPECIAL;
                else if(linha[i]!= ' ' && linha[i]!= '\t' && linha[i]!= '\n'){
                    estado = DENTRO;
                    nome_lido[j] = linha[i];
                    j++;
                }
                break;
            case DENTRO:
                if(linha[i] != ' ' && linha[i] != '\t' && linha[i]!= '\n'){
                    nome_lido[j] = linha[i];
                    j++;
                }
                else 
                    estado = FORA;
                break;
            case ESPECIAL:
                if(linha[i] != '"' && linha[i]!= '\n'){
                    nome_lido[j] = linha[i];
                    j++;
                }
                else
                    estado = FORA;   
                break;     
        }
    }
    nome_lido[j]='\0';
}

/* Mostra as coordenadas geograficas de uma paragem. */
void mostra_paragem(char linha[LINHA_MAX],Paragem *paragens,int num_paragens,
                    Carreira *carreiras, int num_carreiras, Ligacao *ligacoes){
    int i, existe_paragem = 0;
    char *nome_lido = (char*) malloc(sizeof(char) * strlen(linha));
    verifica_memoria(paragens, num_paragens, carreiras, num_carreiras,
                    ligacoes, nome_lido);

    obtem_paragem(linha, nome_lido);
    /* Verifica se a paragem existe. 
    Caso exista então são mostradas as coordenadas da mesma. */
    for(i = 0; i < num_paragens; i++){
        if(strcmp(nome_lido,paragens[i].nome) == 0){
            printf("%16.12f ", paragens[i].latitude);
            printf("%16.12f\n", paragens[i].longitude);
            existe_paragem = 1;
            break;
        }
    }
    /* Se a paragem nao existir a funcao da erro. */
    if(!(existe_paragem))    
        printf("%s: no such stop.\n", nome_lido);
    free(nome_lido);
}

/* Obtem os argumentos necessarios para criar uma paragem. */
void obtem_paragem_arg(char linha[LINHA_MAX], char *nome_lido,
                      char *coordenadas){
    int estado = FORA, comprimento_linha, argumento = 1, i, j = 0;

    comprimento_linha = strlen(linha);
    for(i = 0; argumento == 1; i++){
        switch (estado){
            case FORA:
                if(linha[i] == '"')
                    estado = ESPECIAL;
                else if(linha[i]!= ' ' && linha[i] != '\t'){
                    estado = DENTRO;
                    nome_lido[j] = linha[i];
                    j++;
                }
                break;
            case DENTRO:
                if(linha[i] != ' ' && linha[i] != '\t'){
                    nome_lido[j] = linha[i];
                    j++;
                }
                else {
                    estado = FORA;
                    argumento++;
                }
                break;
            case ESPECIAL:
                if(linha[i] != '"'){
                    nome_lido[j] = linha[i];
                    j++;
                }
                else {
                    estado = FORA;   
                    argumento++;
                }
                break;     
        }
    }
    nome_lido[j] = '\0';
    /* Armazena as coordenadas numa string. */
    for(j = 0; i < comprimento_linha; i++, j++)
        coordenadas[j] = linha[i];                         
}

/* Cria uma nova paragem. */
char* cria_paragem(char linha[LINHA_MAX], Paragem *paragens, int num_paragens,
                Carreira *carreiras, int num_carreiras, Ligacao *ligacoes){
    int tamanho, i;
    char *nome_lido, *coordenadas, *latitude, *longitude;
    nome_lido = (char*) malloc(sizeof(char) * (strlen(linha) + 1));
    verifica_memoria(paragens, num_paragens, carreiras, 
                    num_carreiras,ligacoes,nome_lido);
    coordenadas = (char*)malloc(sizeof(char) * (strlen(linha) + 1));
    verifica_memoria(paragens, num_paragens, carreiras, 
                    num_carreiras, ligacoes, coordenadas);
    latitude = (char*)malloc(sizeof(char) * (strlen(linha) + 1));
    verifica_memoria(paragens, num_paragens, carreiras, 
                    num_carreiras, ligacoes, latitude);
    longitude = (char*)malloc(sizeof(char) * (strlen(linha) + 1));
    verifica_memoria(paragens, num_paragens, carreiras, 
                    num_carreiras, ligacoes, longitude);

    obtem_paragem_arg(linha, nome_lido, coordenadas);
    tamanho = strlen(nome_lido);
    /* Verifica se a paragem que se quer criar ja existe. */
    for(i = 0; i < num_paragens; i++)
        if(paragens[i].nome != NULL)
            if(strcmp(nome_lido, paragens[i].nome) == 0){
                printf("%s: stop already exists.\n", nome_lido);
                free(nome_lido);
                return NULL;
            }
    paragens[num_paragens].nome = (char*)malloc(sizeof(char)*(tamanho + 1));
    verifica_memoria(paragens, num_paragens, carreiras, 
                    num_carreiras, ligacoes, paragens[num_paragens].nome);
    strcpy(paragens[num_paragens].nome, nome_lido);
    /* Obtem a latitude a partir do vetor com as coordenadas. */
    strcpy(latitude, strtok(coordenadas," \t\n"));
    /* Obtem a longitude a partir do vetor com as coordenadas. */
    strcpy(longitude, strtok(NULL," \t\n"));
    /* A latitude e a longitude sao armazenadas no vetor das paragens. */
    paragens[num_paragens].latitude = atof(latitude);
    paragens[num_paragens].longitude = atof(longitude);

    free(nome_lido);
    free(coordenadas);
    free(latitude);
    free(longitude);
    return paragens[num_paragens].nome;
}

/* Obtem os argumentos necessarios para criar a ligacao. */
void obtem_lig_args(char linha[LINHA_MAX], char *carreira, char *origem,
                    char *destino, char *custo_str, char * duracao_str){
    int i, j = 0, argumento = 0, estado = FORA; 
    for(i = 0; argumento <= 4; i++){
            switch (estado){
                case FORA:
                    if(linha[i] == '"')
                        estado = ESPECIAL;
                    else if(linha[i]!=' ' && linha[i]!='\t' && linha[i]!='\n'){
                        estado = DENTRO;
                        if(argumento == 1)
                            origem[j] = linha[i];
                        else if(argumento == 2) 
                            destino[j] = linha[i];
                        else if(argumento == 3)
                            custo_str[j] = linha[i];
                        else
                            duracao_str[j] = linha[i];
                        j++;
                    }
                    break;
                case DENTRO:
                    if(linha[i] != ' ' && linha[i] != '\t' && linha[i]!= '\n'){
                        if(argumento == 1)
                            origem[j] = linha[i];
                        else if(argumento == 2)
                            destino[j] = linha[i];
                        else if(argumento == 3)
                            custo_str[j] = linha[i];
                        else
                            duracao_str[j] = linha[i];
                        j++;
                    }
                    else {
                        estado = FORA;
                        if(argumento == 0){
                            j = 0;
                            i = strlen(carreira) + 1;
                        }
                        else if(argumento == 1){
                            origem[j] = '\0';
                            j = 0;
                        }
                        else if(argumento == 2){
                            destino[j] = '\0';
                            j = 0;
                        }
                        else if(argumento == 3){
                            custo_str[j] = '\0';
                            j = 0;
                        }
                        else {
                            duracao_str[j] = '\0';
                        }
                        argumento++;
                    }
                    break;
                case ESPECIAL:
                    if(linha[i] != '"'){
                        if(argumento == 1)
                            origem[j] = linha[i];
                        else
                            destino[j] = linha[i];
                        j++;
                    }
                    else {
                        estado = FORA;
                        if(argumento == 1){
                            origem[j] = '\0';
                            j = 0;
                        }
                        else {
                            destino[j] = '\0';
                            j = 0;
                        }   
                        argumento++;
                    }
                    break;     
            }
        }    
}

/* Copia para o vetor das ligações cada uma das componentes de uma ligacao. */
void copia_ligacao(Ligacao *ligacoes, Paragem *paragens, Carreira *carreiras,
                  int num_paragens, int lig, int num_carreiras, double custo,
                  char *carreira, char *origem, char *destino, double duracao){
    int i;
    /* lig é o número de ligacoes que existem no vetor das ligacoes. */
    /* As componentes da carreira da ligacao são adicionadas. */
    ligacoes[lig].carreira.nome = (char*)malloc(sizeof(char)*
                                         (strlen(carreira) + 1));
    verifica_memoria(paragens, num_paragens, carreiras, num_carreiras,
                    ligacoes, ligacoes[lig].carreira.nome);
    for(i = 0; i < num_carreiras; i++)
        if(strcmp(carreiras[i].nome, carreira) == 0)
            break;

    strcpy(ligacoes[lig].carreira.nome, carreira);

    ligacoes[lig].carreira.origem.nome = (char*)malloc(sizeof(char) *
                                    (strlen(carreiras[i].origem.nome) + 1));
    verifica_memoria(paragens, num_paragens, carreiras, num_carreiras,
                    ligacoes, ligacoes[lig].carreira.origem.nome);                            
    strcpy(ligacoes[lig].carreira.origem.nome, carreiras[i].origem.nome);

    ligacoes[lig].carreira.origem.latitude = carreiras[i].origem.latitude;
    ligacoes[lig].carreira.origem.longitude = carreiras[i].origem.longitude;

    ligacoes[lig].carreira.destino.nome = (char*)malloc(sizeof(char) *
                                    (strlen(carreiras[i].destino.nome) + 1));
    verifica_memoria(paragens, num_paragens, carreiras, num_carreiras,
                    ligacoes, ligacoes[lig].carreira.destino.nome);
    strcpy(ligacoes[lig].carreira.destino.nome, carreiras[i].destino.nome);

    ligacoes[lig].carreira.destino.latitude = carreiras[i].destino.latitude;
    ligacoes[lig].carreira.destino.longitude = carreiras[i].destino.longitude;

    /* O custo e a duracao sao adicionados. */
    ligacoes[lig].custo = custo;
    ligacoes[lig].duracao = duracao;

    /* As componentes da paragem de origem e de destino 
    da ligacao sao adicionadas.  */
    ligacoes[lig].origem.nome = (char*)malloc(sizeof(char)*(strlen(origem)+1));
    verifica_memoria(paragens, num_paragens, carreiras, num_carreiras,
                    ligacoes, ligacoes[lig].origem.nome);
    strcpy(ligacoes[lig].origem.nome, origem);
    ligacoes[lig].destino.nome=(char*)malloc(sizeof(char)*(strlen(destino)+1));
    verifica_memoria(paragens, num_paragens, carreiras, num_carreiras,
                    ligacoes, ligacoes[lig].destino.nome);
    strcpy(ligacoes[lig].destino.nome, destino);
    for(i = 0; i < num_paragens; i++){
        if(strcmp(origem, paragens[i].nome) == 0){
            ligacoes[lig].origem.latitude = paragens[i].latitude;
            ligacoes[lig].origem.longitude = paragens[i].longitude;
        }
        if(strcmp(destino, paragens[i].nome) == 0){
            ligacoes[lig].destino.latitude = paragens[i].latitude;
            ligacoes[lig].destino.longitude = paragens[i].longitude;
        }
    }
}

/* Atualiza uma carreira quando lhe é adicionada uma ligacao ou dá erro 
caso não seja possível adicionar a ligação. */
int atualiza_carreira(Paragem *paragens, int num_paragens, Carreira *carreiras,
                    int num_carreiras,Ligacao *ligacoes, int num_ligacoes,
                    char *carreira, char *origem, char *destino){
    int i, flag = 1, erros = 0;
    for(i = 0; i < num_ligacoes; i++)
        if(strcmp(carreira, ligacoes[i].carreira.nome) == 0)
            flag = 0;
        
    for(i = 0; i < num_carreiras; i++)
        if(strcmp(carreira, carreiras[i].nome) == 0){
            /* Verifica se a carreira já tem alguma paragem. */
            if(!(flag)){
                /* Se a origem da ligacao for o destino da carreira,
                entao a ligacao é inserida no fim da carreira. */
                if(strcmp(origem, carreiras[i].destino.nome) == 0)
                    strcpy(carreiras[i].destino.nome, destino);

                /* Se o destino da ligacao for a origem da carreira,
                entao a ligacao é inserida no inicio da carreira. */
                else if(strcmp(destino, carreiras[i].origem.nome) == 0)
                    strcpy(carreiras[i].origem.nome, origem);
                    
                else {
                    printf("link cannot be associated with bus line.\n");
                    erros = 1;
                }  
            }
            /* Quando a carreira nao tem paragens os extremos 
            da ligacao passam a ser os extremos da carreira. */
            else {
                carreiras[i].origem.nome = (char*) malloc(sizeof(char)*
                                            (strlen(origem) * 2));
                verifica_memoria(paragens, num_paragens, carreiras, 
                            num_carreiras, ligacoes, carreiras[i].origem.nome);
                strcpy(carreiras[i].origem.nome, origem);
                carreiras[i].destino.nome = (char*) malloc(sizeof(char)*
                                            (strlen(destino) * 2));
                verifica_memoria(paragens, num_paragens, carreiras, 
                            num_carreiras, ligacoes, carreiras[i].origem.nome);
                strcpy(carreiras[i].destino.nome, destino);
            } 
        }
    return erros;
}

/* Adiciona ligacoes a uma carreira. */
int adiciona_ligacoes(char linha[LINHA_MAX], Paragem *paragens, 
                      int num_paragens, Carreira *carreiras, int num_carreiras, 
                      Ligacao *ligacoes, int num_ligacoes){
    double custo, duracao;
    int i, erros = 0, origem_existe = 0, destino_existe = 0;
    char *nome_carreira, *origem, *destino, *custo_str, *duracao_str;

    nome_carreira = (char*) malloc(sizeof(char) * strlen(linha));
    verifica_memoria(paragens, num_paragens, carreiras, num_carreiras,
                    ligacoes, nome_carreira);
    origem = (char*) malloc(sizeof(char) * strlen(linha));
    verifica_memoria(paragens, num_paragens, carreiras, num_carreiras,
                    ligacoes, origem);
    destino = (char*) malloc(sizeof(char) * strlen(linha));
    verifica_memoria(paragens, num_paragens, carreiras, num_carreiras,
                    ligacoes, destino);
    custo_str = (char*) malloc(sizeof(char) * strlen(linha));
    verifica_memoria(paragens, num_paragens, carreiras, num_carreiras,
                    ligacoes,  custo_str);
    duracao_str = (char*) malloc(sizeof(char) * strlen(linha));
    verifica_memoria(paragens, num_paragens, carreiras, num_carreiras,
                    ligacoes, duracao_str);

    nome_carreira = strtok(linha, " \t\n");
    if(!(verifica_carreira(linha, num_carreiras,num_paragens, carreiras,
                        paragens, ligacoes))){
        printf("%s: no such line.\n", nome_carreira);
        free(custo_str);
        free(duracao_str);
        free(origem);
        free(destino);
        return 0;
    }
    else {
        obtem_lig_args(linha, nome_carreira, origem, destino,
                      custo_str, duracao_str);
        /* Verifica se a origem e o destino da ligacao existem. */
        for(i = 0; i < num_paragens; i++){
            if(strcmp(origem, paragens[i].nome) == 0)
                origem_existe++;
            if(strcmp(destino, paragens[i].nome) == 0)
                destino_existe++;
        }
        custo = atof(custo_str);
        duracao = atof(duracao_str);
        if(custo >= 0.0 && duracao >= 0.0 && origem_existe && destino_existe){
            /* Se atualiza_carreira retornar 1, quer dizer que 
            nao foi possivel atualizar a carreira, pois houve um erro. */
            if(atualiza_carreira(paragens, num_paragens,carreiras, 
                                num_carreiras, ligacoes, num_ligacoes, 
                                nome_carreira, origem, destino))
                erros = 1;
        }
        else if(!(origem_existe)){
            erros = 1;
            printf("%s: no such stop.\n", origem);
        }
        else if(!(destino_existe)){
            printf("%s: no such stop.\n", destino);
            erros = 1;
        }
        else {
            printf("negative cost or duration.\n");
            erros = 1;
        }
        
        /* A ligacao eh adicionada ao vetor das ligacoes. */
        if(!(erros)){
            copia_ligacao(ligacoes, paragens, carreiras, num_paragens,
                         num_ligacoes, num_carreiras, custo, nome_carreira,
                         origem, destino, duracao);
            free(custo_str);
            free(duracao_str);
            free(origem);
            free(destino);
            return 1;
        }
        free(custo_str);
        free(duracao_str);
        free(origem);
        free(destino);
        return 0;
    }
}                          

/* Ordena alfabeticamente os nomes das carreiras. */
void ordena_carreiras(char *matriz[], int n, Paragem *paragens, 
                    int num_paragens, Carreira *carreiras, int num_carreiras,
                    Ligacao *ligacoes){
    char *vetor_temporario;
    int i, j;

    /* Selection sort para ordenar strings. */
    for(i = 0; i < n-1; i++){
        int j_min = i;
        for(j = i+1; j < n; j++)
            if(strcmp(matriz[j], matriz[j_min]) < 0)
                j_min = j;
        if(j_min != i){
            vetor_temporario = (char*) malloc(sizeof(char) * 
                                        (strlen(matriz[i]) + 1));
            verifica_memoria(paragens, num_paragens, carreiras, num_carreiras,
                            ligacoes, vetor_temporario);
            strcpy(vetor_temporario, matriz[i]);
            strcpy(matriz[i], matriz[j_min]);
            strcpy(matriz[j_min], vetor_temporario);
            free(vetor_temporario);
        }
    }
}

/*Lista as paragens que correspondem a intersecoes de carreiras. */
void lista_intersecoes(Paragem *paragens, int num_paragens,Carreira *carreiras,
                      int num_carreiras, Ligacao *ligacoes, int num_ligacoes){
    int i,j,l, contador = 0, origem_index, destino_index, carreira_index;
    char **carreiras_das_paragens =(char**)malloc(sizeof(char*)*num_carreiras);
    int **matriz = (int **)calloc(num_paragens, sizeof(int*));
    verifica_memoria(paragens, num_paragens, carreiras, num_carreiras,
                    ligacoes, carreiras_das_paragens);
    verifica_memoria(paragens, num_paragens, carreiras, num_carreiras, 
                    ligacoes, matriz);
    for (i = 0; i < num_paragens; i++){
        matriz[i] = (int *)calloc(num_carreiras, sizeof(int));
        verifica_memoria(paragens, num_paragens, carreiras, num_carreiras, 
                    ligacoes, matriz[i]);
    }
    for (i = 0; i < num_ligacoes; i++) {
        origem_index = obter_index_paragem(paragens, num_paragens, 
                                        ligacoes[i].origem.nome);
        destino_index = obter_index_paragem(paragens, num_paragens, 
                                        ligacoes[i].destino.nome);
        carreira_index = obter_index_carreira(carreiras, num_carreiras, 
                                        ligacoes[i].carreira.nome);
        /* As entradas da matriz que correspondem a paragens que pertencem
        a uma dada carreira têm o valor 1. Caso contrário têm o valor 0. */
        matriz[origem_index][carreira_index] = 1;
        matriz[destino_index][carreira_index] = 1;
    }
    /* As carreiras que são intersetadas por uma dada paragem são guardadas
    e "impressas". */
    for (i = 0; i < num_paragens; i++){
        for(j = 0; j < num_carreiras; j++) 
            if(matriz[i][j] == 1){
                carreiras_das_paragens[contador] = (char*) malloc(
                            sizeof(char) * strlen(carreiras[j].nome) * 2);
                strcpy(carreiras_das_paragens[contador], carreiras[j].nome);
                contador++;
            }
        ordena_carreiras(carreiras_das_paragens, contador, paragens, 
                        num_paragens, carreiras, num_carreiras, ligacoes);
        /* So se mostram paragens que sao intersetadas 
        por mais do que uma carreira. */
        if(contador > 1){
            printf("%s %d:", paragens[i].nome, contador);
            for(l = 0; l < contador - 1; l++){
                printf(" %s", carreiras_das_paragens[l]);
                free(carreiras_das_paragens[l]);
            }
            printf(" %s\n", carreiras_das_paragens[l]);
            free(carreiras_das_paragens[l]);
        }
        contador = 0;
    }
    for (i = 0; i < num_paragens; i++)
        free(matriz[i]);
    free(matriz);
}

/* Remove uma carreira e todas as suas ligações do sistema. */
int remove_carreira(char linha[LINHA_MAX], Paragem *paragens, int num_paragens,
                    Carreira *carreiras, int num_carreiras, Ligacao *ligacoes, 
                    int num_ligacoes){
    int i, contador = 0;
    char *nome_carreira = (char*)malloc(sizeof(char)* (strlen(linha) + 1));

    verifica_memoria(paragens, num_paragens, carreiras, num_carreiras, 
                    ligacoes, nome_carreira);
    strcpy(nome_carreira, strtok(linha, " \n\t"));
    if(verifica_carreira(linha, num_carreiras, num_paragens, carreiras, paragens,
                        ligacoes)){
        /* Remove todas as ligações da carreira do sistema. */
        for(i = num_ligacoes - 1; i >= 0; i--) 
            if(strcmp(nome_carreira, ligacoes[i].carreira.nome) == 0) {
                memmove(ligacoes+i, ligacoes+i+1, 
                        (num_ligacoes-i-1) * sizeof(Ligacao));
                contador++;
                num_ligacoes--;
            }
        /* Remove a carreira do sistema. */
        for(i = 0; i < num_carreiras; i++)
            if(strcmp(carreiras[i].nome, nome_carreira) == 0){
                for(; i < num_carreiras - 1; i++)
                    carreiras[i] = carreiras[i+1];
                contador++;
                break;
            }
    }
    /* Se a carreira não existe a função dá erro. */
    else
        printf("%s: no such line.\n", nome_carreira);
    free(nome_carreira);
    return contador;
}

/* Elimina uma paragem do sistema e atualiza todas as ligações que contenham
essa paragem. */
int elimina_paragem(char linha[LINHA_MAX], Paragem *paragens, int num_paragens, 
                    Ligacao *ligacoes, int num_ligacoes, Carreira *carreiras, 
                    int num_carreiras){
    int i, j, k = -1,l = -1, contador = 0,flag = 1;
    char *nome_paragem = (char*)malloc(sizeof(char)* strlen(linha) + 1);

    verifica_memoria(paragens, num_paragens, carreiras, num_carreiras,
                    ligacoes, nome_paragem);
    obtem_paragem(linha, nome_paragem);
    /* Elimina a paragem do sistema. */
    for(i = num_paragens-1; i >= 0; i--){
        if(strcmp(nome_paragem, paragens[i].nome) == 0){
            flag = 0;
            for(;i < num_paragens-1; i++)
                paragens[i] = paragens[i+1];
            contador++;
            break;
        }
    }
    if(!(flag)){
        /* Atualiza as ligações que contêm a paragem. */
        for(i = 0; i < num_carreiras; i++){
            for(j = 0; j < num_ligacoes; j++){
                if(strcmp(carreiras[i].nome, ligacoes[j].carreira.nome) == 0){
                    flag = 0;
                    /* k armazena o indice de uma ligação da carreira, que 
                    tenha a paragem que se quer eliminar, na origem. */
                    if(k == -1 && strcmp(nome_paragem,
                                        ligacoes[j].origem.nome) == 0)
                        k = j;
                    /* l armazena o indice de uma ligação da carreira, que 
                    tenha a paragem que se quer eliminar, no destino. */
                    if(l == -1 && strcmp(nome_paragem,
                                        ligacoes[j].destino.nome) == 0)
                        l = j;
                    /* A ligação não contêm a paragem. */
                    if(k == -1 && l == -1)
                        continue;
                    /* A paragem é a origem da ligação e da carreira em 
                    simultâneo. Nesse caso elimina-se a ligação e atualiza-se
                    as informações da origem da carreira. */
                    else if(l == -1 && strcmp(carreiras[i].origem.nome,
                                             nome_paragem) == 0){
                        strcpy(carreiras[i].origem.nome,
                                ligacoes[k].destino.nome);  
                        carreiras[i].origem.latitude = ligacoes[k].
                                                        destino.latitude;
                        carreiras[i].origem.longitude = ligacoes[k].
                                                        destino.longitude;
                        j = k - 1;
                        memmove(&ligacoes[k], &ligacoes[k+1], 
                                sizeof(Ligacao) * (num_ligacoes - k - 1));
                        num_ligacoes--;
                        contador++;
                        k = l = -1;
                    }
                    /* A paragem é o destino da ligação e da carreira em 
                    simultâneo. Nesse caso elimina-se a ligação e atualiza-se
                    as informações do destino da carreira. */  
                    else if(k == -1 && strcmp(carreiras[i].destino.nome, 
                                            nome_paragem) == 0){
                        strcpy(carreiras[i].destino.nome, ligacoes[l].
                                                        origem.nome);
                        carreiras[i].destino.latitude = ligacoes[l].
                                                        origem.latitude;
                        carreiras[i].destino.longitude = ligacoes[l].
                                                        origem.longitude;
                        j = l - 1;
                        memmove(&ligacoes[l], &ligacoes[l+1], 
                                sizeof(Ligacao) * (num_ligacoes - l - 1));
                        contador++;
                        num_ligacoes--;
                        k = l = -1;   
                    }
                    /* Uma ligação tem a paragem na origem e outra tem-na no
                    destino. Neste caso juntam-se as ligações "saltando" a 
                    paragem. */
                    else if(k != -1 && l != -1){
                        if(strcmp(ligacoes[l].destino.nome, 
                                            ligacoes[k].destino.nome) != 0){
                            strcpy(ligacoes[l].destino.nome, 
                                    ligacoes[k].destino.nome);
                        }
                        ligacoes[l].destino.latitude = ligacoes[k].destino.
                                                                    latitude;
                        ligacoes[l].destino.longitude = ligacoes[k].destino.
                                                                    longitude;
                        ligacoes[l].custo += ligacoes[k].custo;
                        ligacoes[l].duracao += ligacoes[k].duracao;
                        memmove(&ligacoes[k], &ligacoes[k+1], 
                                sizeof(Ligacao) * (num_ligacoes - k - 1));
                        contador++; 
                        num_ligacoes--;
                        j = l-1;
                        k = l = -1;
                    }
                }
            }
            k = l = -1;
        }
        free(nome_paragem);
        return contador;
    }
    /* Se a paragem não existe a função dá erro. */
    else {
        printf("%s: no such stop.\n", nome_paragem);
        free(nome_paragem);
        return contador;
    }
}

