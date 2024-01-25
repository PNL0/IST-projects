/* 
 * Ficheiro: project1.c

 * Autor: Pedro Nunes Leal (ist1106154)

 * Descricao: Este programa funciona como um sistema de gestão 
 * de carreiras de transporte público, onde pode-se definir e consultar
 * estações (paragens) e percursos.   
*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
/* Número de bytes máximo de uma linha de input. */
#define LINHA_MAX BUFSIZ
/* Tamanho máximo de alguma abreviatura da palavra "inverso" */
#define PALAVRA_INVERSO 20
/* Número de caracteres máximo do nome de uma paragem. */
#define NOME_PARAGEM_MAX 51
/* Número de caracteres máximo do nome de uma carreira. */
#define NOME_CARREIRA_MAX 21
/* Número máximo de paragens no sistema. */
#define PARAGENS_MAX 10000
/* Número máximo de carreiras no sistema. */
#define CARREIRAS_MAX 200
/* Numero maximo de ligacoes no sistema. */
#define LIGACOES_MAX 30000
/* Tamanho maximo de cada uma das coordenadas de uma paragem. */
#define COORDENADAS_MAX 40
/* Numero maximo de algarismos para o custo de uma ligacao*/
#define CUSTO 20
/* Numero maximo de algarismos para a duracao de uma ligacao. */
#define DURACAO 20

typedef struct paragem {
    char nome[NOME_PARAGEM_MAX]; /* Nome da paragem. */
    double latitude;   /* Latitude da paragem. */
    double longitude;  /* Longitude da paragem. */
} Paragem;
Paragem paragens[PARAGENS_MAX]; /* Vetor que armazena as paragens. */

typedef struct carreira {
    char nome[NOME_CARREIRA_MAX]; /* Nome da carreira. */
    Paragem origem;   /* Paragem de origem da carreira. */
    Paragem destino;  /* Paragem de destino da carreira. */
} Carreira;
Carreira carreiras[CARREIRAS_MAX]; /* Vetor que armazena as carreiras. */

typedef struct ligacao {
    Carreira carreira; /* Carreira da qual a ligacao faz parte. */
    Paragem origem;    /* Paragem de origem da ligacao. */
    Paragem destino;   /* Paragem de destino da ligacao. */
    double custo;      /* Custo da ligacao. */
    double duracao;    /* Duracao da ligacao. */
} Ligacao;
Ligacao ligacoes[LIGACOES_MAX]; /* Vetor que armazena as ligacoes. */

enum estado{
    FORA, /* Quando o caracter lido nao faz parte de nenhum argumento. */
    DENTRO, /* Quando o caracter lido faz parte de algum argumento. */
    ESPECIAL /* Quando o caracter lido faz parte do nome de uma paragem,
                estando este nome delimitado por aspas. */
};

int conta_argumentos(char linha[LINHA_MAX]);

int verifica_carreira(char linha[LINHA_MAX], int num_carreiras);

void lista_carreiras(int num_carreiras, int num_ligacoes);

void mostra_carreira(char linha[LINHA_MAX],int num_carreiras,int num_ligacoes);

void cria_carreira(char linha[LINHA_MAX], int num_carreiras);

void mostra_carreira_inversa(char linha[LINHA_MAX], int num_carreiras,
                            int num_ligacoes);

void lista_paragens(int num_paragens, int num_carreiras, int num_ligacoes);

void obtem_paragem(char linha[LINHA_MAX], char nome_lido[NOME_PARAGEM_MAX]);

void mostra_paragem(char linha[LINHA_MAX], int num_paragens);

void obtem_paragem_arg(char linha[LINHA_MAX], char nome_lido[NOME_PARAGEM_MAX],
                      char coordenadas[COORDENADAS_MAX]);

void cria_paragem(char linha[LINHA_MAX], int num_paragens);

void obtem_lig_args(char linha[LINHA_MAX], char carreira[NOME_CARREIRA_MAX],
                   char origem[NOME_PARAGEM_MAX],char destino[NOME_PARAGEM_MAX]
                   , char custo_str[CUSTO], char duracao_str[DURACAO]);

void copia_ligacao(int lig, int i, int num_paragens, double custo,
                  char origem[NOME_PARAGEM_MAX],char destino[NOME_PARAGEM_MAX],
                  double duracao, char carreira[NOME_CARREIRA_MAX]);

int atualiza_carreira(int num_carreiras, char carreira[NOME_CARREIRA_MAX],
                      char origem[NOME_PARAGEM_MAX], 
                      char destino[NOME_PARAGEM_MAX]);

void adiciona_ligacoes(char linha[LINHA_MAX], int num_paragens,
                      int num_carreiras, int num_ligacoes);

void ordena_carreiras(char matriz[CARREIRAS_MAX][NOME_CARREIRA_MAX], int n);

void lista_intersecoes(int num_paragens, int num_carreiras, int num_ligacoes);

/* Funciona de acordo com os comandos e argumentos que le no input. */
int main() {
    char linha[LINHA_MAX]; 
    int comando,argumentos,num_ligacoes = 0,num_carreiras = 0,num_paragens = 0;
    /* O comando é sempre o primeiro caracter de cada linha de input. */
    /* O comando q termina o programa. */
    while((comando = getchar()) != 'q'){
        /* O resto da linha de input contem os argumentos que foram 
        associados a um dado comando. */
        if(fgets(linha,LINHA_MAX, stdin)!= NULL){ 
            argumentos = conta_argumentos(linha);
            /* O comando c adiciona e lista as carreiras. */
            if(comando == 'c'){
                if(argumentos == 0)
                    lista_carreiras(num_carreiras, num_ligacoes);
                else if(argumentos == 1){
                    /* So se pretende mostrar carreiras que existam. */
                    if(verifica_carreira(linha, num_carreiras))
                        mostra_carreira(linha, num_carreiras, num_ligacoes);
                    else {   
                        cria_carreira(linha, num_carreiras);
                        num_carreiras++;
                    }
                }
                else if(argumentos == 2)
                    mostra_carreira_inversa(linha,num_carreiras, num_ligacoes);
            }
            /* O comando p adiciona e lista as paragens. */
            else if(comando == 'p'){
                if(argumentos == 0)
                    lista_paragens(num_paragens, num_carreiras, num_ligacoes);
                else if(argumentos == 1)
                    mostra_paragem(linha, num_paragens);
                else if(argumentos == 3){
                    cria_paragem(linha, num_paragens);
                    num_paragens++;
                }
            }
            /* O comando l adiciona ligacoes de paragens as carreiras. */
            else if(comando == 'l' && argumentos == 5){
                adiciona_ligacoes(linha, num_paragens, num_carreiras, 
                                 num_ligacoes);
                num_ligacoes++;
            }
            /* O comando i lista os nos de interligacao. */
            else if(comando == 'i' && argumentos == 0)
                lista_intersecoes(num_paragens, num_carreiras, num_ligacoes);
        }
    }
    return 0;
}

/* Conta o numero de argumentos de uma linha. */
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
int verifica_carreira(char linha[LINHA_MAX], int num_carreiras){
    char nome_carreira[NOME_CARREIRA_MAX];
    int i;
    /* Obtem o nome da carreira a partir do input. */
    strcpy(nome_carreira, strtok(linha, " \n\t"));
    /* Procura no vetor das carreiras do sistema o nome de carreira lido. */
    for(i = 0; i < num_carreiras; i++)
        if(strcmp(nome_carreira, carreiras[i].nome) == 0)
            return 1;
    return 0;
}

/* Lista todas as carreiras do sistema. */
void lista_carreiras(int num_carreiras, int num_ligacoes){
    int i, j, contador = 0, num_paragens;
    double custo = 0.00, duracao = 0.00;
    /* Para cada carreira eh obtido o custo total, a duracao total
    e o numero de paragens. */
    for(i = 0; i < num_carreiras; i++){
        for(j = 0; j < num_ligacoes; j++){
            if(strcmp(carreiras[i].nome, ligacoes[j].carreira.nome) == 0){
                custo += ligacoes[j].custo;
                duracao += ligacoes[j].duracao;
                contador++;
            }
        }
        /* O numero de paragens de uma carreira é sempre
        o numero de ligacoes + 1. */
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

/* Lista todas as paragens de uma carreira, desde a origem ate ao destino. */
void mostra_carreira(char linha[LINHA_MAX],int num_carreiras,int num_ligacoes){
    char nome[NOME_CARREIRA_MAX];
    char origem[NOME_PARAGEM_MAX], destino[NOME_PARAGEM_MAX];
    int i;
    /* Obtem o nome da carreira que se pretende mostrar. */
    strcpy(nome, strtok(linha, " "));
    /* Obtem a paragem de origem e de destino da carreira. */
    for(i = 0; i < num_carreiras; i++)
        if(strcmp(nome, carreiras[i].nome) == 0){
            strcpy(origem, carreiras[i].origem.nome);
            strcpy(destino, carreiras[i].destino.nome);
            break;
        }
    if(strcmp(origem, "") != 0){
        printf("%s, ", origem);
        /* Procura a ligacao que tem como paragem de origem a ultima 
        paragem que foi "printada" e que esta armazenada em origem. */
        for(i = 0; i < num_ligacoes; i++)
            if(strcmp(nome, ligacoes[i].carreira.nome) == 0)
                /* Quando encontra a ligacao atualiza o vetor origem
                para a paragem seguinte da carreira (que é a paragem 
                de destino da ligacao). */
                if(strcmp(origem, ligacoes[i].origem.nome) == 0){
                    strcpy(origem, ligacoes[i].destino.nome);
                    /* Enquanto a paragem "printada" nao for a paragem
                    de destino da carreira, entao o ciclo recomeca. */
                    if(strcmp(origem, destino) != 0){
                        printf("%s, ",origem);
                        i = -1;
                    }
                    /* Quando a ultima paragem da carreira eh "printada",
                    entao ja foram mostradas todas as paragens da carreira. */
                    else {
                        printf("%s\n", destino);
                        break;
                    }
                }    
    }
}

/* Cria uma nova carreira. */
void cria_carreira(char linha[LINHA_MAX], int num_carreiras){
    strcpy(carreiras[num_carreiras].nome, strtok(linha, " \n\t"));
} 

/* Lista todas as paragens de uma carreira, desde o destino ate a origem. */
void mostra_carreira_inversa(char linha[LINHA_MAX],int num_carreiras, 
                            int num_ligacoes){
    char nome[NOME_CARREIRA_MAX], inv[PALAVRA_INVERSO];
    char origem[NOME_PARAGEM_MAX], destino[NOME_PARAGEM_MAX];
    int i;
    /* Obtem o nome da carreira que se pretende mostrar. */
    strcpy(nome, strtok(linha, " \t\n"));
    /* Obtem uma abreviatura da palavra "inverso". */
    strcpy(inv, strtok(NULL, " \n\t"));
    /* Verifica se a abreviatura lida eh valida. */
    if (strcmp(inv, "inverso") == 0 || strcmp(inv, "invers") == 0 || 
        strcmp(inv, "inver") == 0 || strcmp(inv, "inve") == 0 || 
        strcmp(inv, "inv") == 0){
        /* Obtem a paragem de origem e de destino da carreira. */
        for(i = 0; i < num_carreiras; i++)
            if(strcmp(nome, carreiras[i].nome) == 0){
                strcpy(origem, carreiras[i].origem.nome);
                strcpy(destino, carreiras[i].destino.nome);
                break;
            }
        if(strcmp(destino, "") != 0){
            printf("%s, ", destino);
            /* Procura a ligacao que tem como paragem de destino a ultima 
            paragem que foi "printada" e que esta armazenada em destino. */
            for(i = 0; i < num_ligacoes; i++)
                if(strcmp(nome, ligacoes[i].carreira.nome) == 0)
                    /* Quando encontra a ligacao atualiza o vetor destino
                    para a paragem anterior da carreira (que é a paragem 
                    de origem da ligacao). */
                    if(strcmp(destino, ligacoes[i].destino.nome) == 0){
                        strcpy(destino, ligacoes[i].origem.nome);
                        /* Enquanto a paragem "printada" nao for a paragem
                        de origem da carreira, entao o ciclo recomeca. */
                        if(strcmp(origem, destino) != 0){
                            printf("%s, ",destino);
                            i = -1;
                        }
                        /* Quando a primeira paragem da carreira 
                        eh "printada", entao ja foram mostradas
                        todas as paragens da carreira. */
                        else {
                            printf("%s\n", origem);
                            break;
                        }
                    } 
        }
    }
    else   
        printf("incorrect sort option.\n");
}

/* Lista todas as paragens do sistema. */
void lista_paragens(int num_paragens, int num_carreiras, int num_ligacoes){                      
    int i, j, k, contador = 0;

    for (i = 0; i < num_paragens; i++){
        if(strcmp(paragens[i].nome, "") != 0){
            printf("%s: ",paragens[i].nome);
            printf("%16.12f ",paragens[i].latitude);
            printf("%16.12f",paragens[i].longitude);
            /* Os dois ciclos seguintes contam o 
            numero de carreiras para uma paragem. */
            /* Para uma paragem pertencer a uma carreira, esta tem de ser
            o destino da carreira ou entao eh a origem de uma ligacao 
            da carreira. */
            for(j = 0; j < num_carreiras; j++){
                if(strcmp(paragens[i].nome, carreiras[j].destino.nome) == 0)
                    /* Eh necessario garantir que carreira que esta a ser
                    avaliada nao eh circular, pois senao esta poderia
                    ser contada duas vezes, caso a paragem avaliada fosse
                    a origem e o destino da carreira em simultaneo. */
                    if(strcmp(paragens[i].nome, carreiras[j].origem.nome)!= 0)
                        contador++;
            }
            for (k = 0; k < num_ligacoes; k++){
                if(strcmp(paragens[i].nome, ligacoes[k].origem.nome) == 0)
                    contador++;
            }   
        printf(" %d\n", contador); 
        }
        contador = 0;
    }
}

/* Guarda o nome de uma paragem. */
void obtem_paragem(char linha[LINHA_MAX], char nome_lido[NOME_PARAGEM_MAX]){
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
void mostra_paragem(char linha[LINHA_MAX],int num_paragens){
    char nome_lido[NOME_PARAGEM_MAX];
    int i, existe_paragem = 0;
    /* Guarda o nome da paragem que se quer mostrar. */
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
}

/* Obtem os argumentos necessarios para criar uma paragem. */
void obtem_paragem_arg(char linha[LINHA_MAX], char nome_lido[NOME_PARAGEM_MAX],
                      char coordenadas[COORDENADAS_MAX]){
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
void cria_paragem(char linha[LINHA_MAX], int num_paragens){
    char nome_lido[NOME_PARAGEM_MAX], coordenadas[COORDENADAS_MAX];
    char latitude[COORDENADAS_MAX], longitude[COORDENADAS_MAX];
    int tamanho_nome, i, valor = 1;

    /* Obtem os argumentos necessarios para criar a paragem. */
    obtem_paragem_arg(linha, nome_lido, coordenadas);
    tamanho_nome = strlen(nome_lido);
    /* Verifica se a paragem que se quer criar ja existe. */
    for(i = 0; i < num_paragens; i++)
        if(strcmp(nome_lido,paragens[i].nome) == 0){
            printf("%s: stop already exists.\n", nome_lido);
            valor = 0;
            break;
        }
    if(valor){
        /* Copia o nome caracter a caracter para o vetor das paragens. */
        for(i = 0; i < tamanho_nome; i++)
            paragens[num_paragens].nome[i] = nome_lido[i];
        /* Obtem a latitude a partir do vetor com as coordenadas. */
        strcpy(latitude, strtok(coordenadas," "));
        /* Obtem a longitude a partir do vetor com as coordenadas. */
        strcpy(longitude, strtok(NULL," "));
        /* A latitude e a longitude sao armazenadas no vetor das paragens. */
        paragens[num_paragens].latitude = atof(latitude);
        paragens[num_paragens].longitude = atof(longitude);
    }
}

/* Obtem os argumentos necessarios para criar a ligacao. */
void obtem_lig_args(char linha[LINHA_MAX], char carreira[NOME_CARREIRA_MAX],
                   char origem[NOME_PARAGEM_MAX],char destino[NOME_PARAGEM_MAX]
                   ,char custo_str[CUSTO], char duracao_str[DURACAO]){

    int i, j = 0, argumento = 0, estado = FORA; 
    for(i = 0; argumento <= 4; i++){
            switch (estado){
                case FORA:
                    if(linha[i] == '"')
                        estado = ESPECIAL;
                    else if(linha[i]!=' ' && linha[i]!='\t' && linha[i]!='\n'){
                        estado = DENTRO;
                        if(argumento == 0)
                            carreira[j] = linha[i];
                        else if(argumento == 1)
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
                        if(argumento == 0)
                            carreira[j] = linha[i];
                        else if(argumento == 1)
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
                            carreira[j] = '\0';
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
void copia_ligacao(int lig, int i, int num_paragens, double custo,
                  char origem[NOME_PARAGEM_MAX],char destino[NOME_PARAGEM_MAX],
                  double duracao, char carreira[NOME_CARREIRA_MAX]){

    /* lig é o número de ligacoes que existem no vetor das ligacoes. */
    /* As componentes da carreira da qual a ligacao faz parte sao adicionadas. */
    strcpy(ligacoes[lig].carreira.nome, carreira);
    strcpy(ligacoes[lig].carreira.origem.nome,carreiras[i].origem.nome);
    ligacoes[lig].carreira.origem.latitude = carreiras[i].origem.latitude;
    ligacoes[lig].carreira.origem.longitude = carreiras[i].origem.longitude;
    strcpy(ligacoes[lig].carreira.destino.nome, carreiras[i].destino.nome);
    ligacoes[lig].carreira.destino.latitude = carreiras[i].destino.latitude;
    ligacoes[lig].carreira.destino.longitude = carreiras[i].destino.longitude;

    /* O custo e a duracao sao adicionados. */
    ligacoes[lig].custo = custo;
    ligacoes[lig].duracao = duracao;

    /* As componentes da paragem de origem e de destino da ligacao sao adicionadas.  */
    strcpy(ligacoes[lig].origem.nome, origem);
    strcpy(ligacoes[lig].destino.nome, destino);
    for(i = 0; i < num_paragens; i++){
        if(strcmp(origem, paragens[i].nome) == 0){
            ligacoes[lig].origem.latitude = paragens[i].latitude;
            ligacoes[lig].origem.longitude = paragens[i].longitude;
        }
        else if(strcmp(destino, paragens[i].nome) == 0){
            ligacoes[lig].destino.latitude = paragens[i].latitude;
            ligacoes[lig].destino.longitude = paragens[i].longitude;
        }
    }
}

/* Atualiza uma carreira quando lhe é adicionada uma ligacao ou dá erro 
caso não seja possível adicionar a ligação. */
int atualiza_carreira(int num_carreiras, char carreira[NOME_CARREIRA_MAX],
                      char origem[NOME_PARAGEM_MAX], 
                      char destino[NOME_PARAGEM_MAX]){
    int i, erros = 0;
    for(i = 0; i < num_carreiras; i++)
        if(strcmp(carreira, carreiras[i].nome) == 0){
            /* Verifica se a carreira já tem alguma paragem. */
            if(strlen(carreiras[i].origem.nome) != 0){
                /* Se a origem da ligacao for o destino da carreira,
                entao a ligacao eh inserida no fim da carreira. */
                if(strcmp(origem, carreiras[i].destino.nome) == 0){
                    strcpy(carreiras[i].destino.nome, destino);
                    break;
                }
                /* Se o destino da ligacao for a origem da carreira,
                entao a ligacao eh inserida no inicio da carreira. */
                if(strcmp(destino, carreiras[i].origem.nome) == 0){
                    strcpy(carreiras[i].origem.nome, origem);
                    break;
                }
                else {
                    printf("link cannot be associated with bus line.\n");
                    erros = 1;
                    break;
                }  
            }
            /* Quando a carreira nao tem paragens os extremos 
            da ligacao passam a ser os extremos da carreira. */
            else {
                strcpy(carreiras[i].origem.nome, origem);
                strcpy(carreiras[i].destino.nome, destino);
            } 
        }
    return erros;
}

/* Adiciona ligacoes a uma carreira. */
void adiciona_ligacoes(char linha[LINHA_MAX],int num_paragens, 
                      int num_carreiras,int n_ligacoes){
    char carreira[NOME_CARREIRA_MAX], origem[NOME_PARAGEM_MAX];
    char destino[NOME_PARAGEM_MAX], custo_str[CUSTO], duracao_str[DURACAO];
    double custo, duracao;
    int i, erros = 0, origem_existe = 0, destino_existe = 0;
    
    if(!(verifica_carreira(linha, num_carreiras)))
        printf("%s: no such line.\n", carreira);
    else {
        /* Obtem os argumentos necessários para criar a ligacao. */
        obtem_lig_args(linha,carreira,origem,destino,custo_str,duracao_str);
        /* Verifica se a origem e o destino da ligacao existem. */
        for(i = 0; i < num_paragens; i++){
            if(strcmp(origem, paragens[i].nome) == 0)
                origem_existe++;
            else if(strcmp(destino, paragens[i].nome) == 0)
                destino_existe++;
        }
        custo = atof(custo_str);
        duracao = atof(duracao_str);
        if(custo >= 0.0 && duracao >= 0.0 && origem_existe && destino_existe){
            /* Se atualiza_carreira retornar 1, quer dizer que 
            nao foi possivel atualizar a carreira, pois houve um erro. */
            if(atualiza_carreira(num_carreiras, carreira, origem, destino))
                erros = 1;
        }
        else if (custo < 0.00 || duracao < 0.00){
            printf("negative cost or duration.\n");
            erros = 1;
        }
        else {
            erros = 1;
            if(!(origem_existe))
                printf("%s: no such stop.\n", origem);
            else 
                printf("%s: no such stop.\n", destino);
        }
        /* A ligacao eh adicionada ao vetor das ligacoes. */
        if(!(erros))
            copia_ligacao(n_ligacoes, i, num_paragens, custo, origem, destino,
                         duracao, carreira);
    }
}                          

/* Ordena alfabeticamente os nomes das carreiras. */
void ordena_carreiras(char matriz[CARREIRAS_MAX][NOME_CARREIRA_MAX], int n){
    char vetor_temporario[NOME_CARREIRA_MAX];
    int i, j;

    /* Selection sort para ordenar strings. */
    for(i = 0; i < n-1; i++){
        int j_min = i;
        for(j = i+1; j < n; j++)
            if(strcmp(matriz[j], matriz[j_min]) < 0)
                j_min = j;
        if(j_min != i){
            strcpy(vetor_temporario, matriz[i]);
            strcpy(matriz[i], matriz[j_min]);
            strcpy(matriz[j_min], vetor_temporario);
        }
    }
}

/* Lista as paragens que correspondem a intersecoes de carreiras. */
void lista_intersecoes(int num_paragens, int num_carreiras, int num_ligacoes){
    int i,j,k,l, contador = 0, bandeira = 0;
    char carreiras_das_paragens[CARREIRAS_MAX][NOME_CARREIRA_MAX];

    for(i = 0; i < num_paragens; i++){
        /* Verifica e armazena as carreiras que contem uma dada paragem. */
        for(j = 0; j < num_carreiras; j++){
            /* Uma carreira contem uma dada paragem, 
            se esta for a sua origem. */
            if(strcmp(paragens[i].nome, carreiras[j].origem.nome) == 0)
                contador++;
            /* Uma carreira contem uma dada paragem, 
            se esta for o seu destino. */
            else if(strcmp(paragens[i].nome, carreiras[j].destino.nome) == 0)
                contador++;
            else 
                for(k = 0; k < num_ligacoes; k++)
                    /* A paragem faz parte da carreira, se pertencer
                    a uma ligacao que esteja na carreira. */
                    if(strcmp(carreiras[j].nome, 
                              ligacoes[k].carreira.nome) == 0){   
                        if(strcmp(paragens[i].nome, 
                                  ligacoes[k].origem.nome) == 0){
                            contador++; 
                            break;
                        }
                        else if(strcmp(paragens[i].nome, 
                                       ligacoes[k].destino.nome) == 0){
                            contador++;
                            break;
                        }
                    }
            if(contador - bandeira){
                strcpy(carreiras_das_paragens[contador-1], carreiras[j].nome);
                bandeira++;
            }
        }
        ordena_carreiras(carreiras_das_paragens, contador);
        /* So se mostram paragens que sao intersetadas 
        por mais do que uma carreira. */
        if(contador > 1){
            printf("%s %d:", paragens[i].nome, contador);
            for(l = 0; l < contador - 1; l++)
                printf(" %s", carreiras_das_paragens[l]);
            printf(" %s\n", carreiras_das_paragens[l]);
        }
        bandeira = 0;
        contador = 0;
    }
}
