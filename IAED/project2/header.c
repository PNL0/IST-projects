/* 
* Ficheiro: project2.h

* Autor: Pedro Nunes Leal (ist1106154)

* Descrição: Este ficheiro contem: as estruturas que definem as paragens,
* carreiras e ligações, os estados a que uma função que leia input pode estar,
* uma constante para o tamanho máximo de uma linha de input e os protótipos das
* funções que foram usadas no projeto. 
*/

#ifndef _PROJECT2_H_
#define _PROJECT2_H_
/* Número de caracteres máximo de uma linha de input. */
#define LINHA_MAX 65536

typedef struct paragem {
    char *nome; 
    double latitude;   
    double longitude;  
} Paragem;

typedef struct carreira {
    char *nome; 
    Paragem origem;   
    Paragem destino; 
} Carreira;

typedef struct ligacao {
    Carreira carreira; 
    Paragem origem;    
    Paragem destino;   
    double custo;      
    double duracao;    
} Ligacao;

enum estado{
    FORA, /* Quando o caracter lido nao faz parte de nenhum argumento. */      
    DENTRO, /* Quando o caracter lido faz parte de algum argumento. */
    ESPECIAL /* Quando o caracter lido faz parte do nome de uma paragem,
                estando este nome delimitado por aspas. */
};

void limpa_vetores(Paragem *paragens, int num_paragens, Carreira *carreiras,
                    int num_carreiras, Ligacao *ligacoes);

void verifica_memoria(Paragem *paragens, int num_paragens, Carreira *carreiras,
                    int num_carreiras, Ligacao *ligacoes, void *ponteiro);

int conta_argumentos(char linha[LINHA_MAX]);

int verifica_carreira(char linha[LINHA_MAX],int num_carreiras,int num_paragens,
                     Carreira *carreiras, Paragem *paragens,Ligacao *ligacoes);

void lista_carreiras(Carreira *carreiras, int num_carreiras, 
                    Ligacao *ligacoes, int num_ligacoes);

int verifica_ordem(Ligacao *ligacoes, int num_ligacoes, char *nome_carreira,
                char *origem, char *destino);

int imprime_paragens_1(Ligacao *ligacoes, char *nome_carreira, char *origem, 
                     char *destino, int i, int contador);

int imprime_paragens_2(Ligacao *ligacoes, char *nome_carreira, char *origem, 
                     char *destino, int i, int contador);

void mostra_carreira(char linha[LINHA_MAX], Paragem *paragens,int num_paragens,
                    Carreira *carreiras, int num_carreiras,
                    Ligacao *ligacoes, int num_ligacoes);

char* cria_carreira(char linha[LINHA_MAX], Paragem *paragens, int num_paragens,
                    Carreira *carreiras, int num_carreiras, Ligacao *ligacoes);

void mostra_carreira_inversa(char linha[LINHA_MAX], Paragem *paragens,
                        int num_paragens, Carreira *carreiras, 
                        int num_carreiras, Ligacao *ligacoes,int num_ligacoes);

int obter_index_paragem(Paragem *paragens,int num_paragens,char *nome_paragem);

int obter_index_carreira(Carreira *carreiras,int num_carreiras,
                        char *nome_carreira);

void lista_paragens(Paragem *paragens, int num_paragens, Carreira *carreiras,
                   int num_carreiras, Ligacao *ligacoes, int num_ligacoes);

void obtem_paragem(char linha[LINHA_MAX], char *nome_lido);

void mostra_paragem(char linha[LINHA_MAX],Paragem *paragens,int num_paragens,
                    Carreira *carreiras, int num_carreiras, Ligacao *ligacoes);

void obtem_paragem_arg(char linha[LINHA_MAX], char *nome_lido,
                    char *coordenadas);

char* cria_paragem(char linha[LINHA_MAX], Paragem *paragens, int num_paragens,
                Carreira *carreiras, int num_carreiras, Ligacao *ligacoes);

void obtem_lig_args(char linha[LINHA_MAX], char *carreira, char *origem,
                    char *destino, char *custo_str, char * duracao_str);

void copia_ligacao(Ligacao *ligacoes, Paragem *paragens, Carreira *carreiras,
                int num_paragens, int lig, int num_carreiras, double custo,
                char *carreira, char *origem, char *destino, double duracao);

int atualiza_carreira(Paragem *paragens, int num_paragens, Carreira *carreiras,
                    int num_carreiras,Ligacao *ligacoes, int num_ligacoes,
                    char *carreira, char *origem, char *destino);

int adiciona_ligacoes(char linha[LINHA_MAX], Paragem *paragens, 
                    int num_paragens, Carreira *carreiras, int num_carreiras, 
                    Ligacao *ligacoes, int num_ligacoes);

void ordena_carreiras(char *matriz[], int n, Paragem *paragens, 
                    int num_paragens, Carreira *carreiras, int num_carreiras,
                    Ligacao *ligacoes);

void lista_intersecoes(Paragem *paragens, int num_paragens,Carreira *carreiras,
                    int num_carreiras, Ligacao *ligacoes, int num_ligacoes);

int remove_carreira(char linha[LINHA_MAX], Paragem *paragens, int num_paragens,
                    Carreira *carreiras, int num_carreiras, Ligacao *ligacoes, 
                    int num_ligacoes);

int elimina_paragem(char linha[LINHA_MAX], Paragem *paragens, int num_paragens, 
                    Ligacao *ligacoes, int num_ligacoes, Carreira *carreiras, 
                    int num_carreiras);

#endif
