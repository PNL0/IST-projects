/* 
* Ficheiro: project2.c

* Autor: Pedro Nunes Leal (ist1106154)

* Descrição: Este programa funciona como um sistema de gestão
* de carreiras de transporte público, onde pode-se definir, 
* consultar e eliminar estações (paragens) e percursos.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "project2.h"

/* Funciona de acordo com os comandos e argumentos que le no input. */
int main() {
    /* Paragens é o vetor onde serão armazenadas as paragens do sistema. */
    Paragem *paragens = (Paragem*) malloc(sizeof(Paragem));
    Paragem *paragens_aux = NULL;
    /* Carreiras é o vetor onde serão armazenadas as carreiras do sistema. */
    Carreira *carreiras = (Carreira*) malloc(sizeof(Carreira)); 
    Carreira *carreiras_aux = NULL;
    /* Ligacoes é o vetor onde serão armazenadas as ligações do sistema. */
    Ligacao *ligacoes = (Ligacao*) malloc(sizeof(Ligacao)); 
    Ligacao *ligacoes_aux = NULL;
    char linha[LINHA_MAX]; 
    int comando, argumentos, i;
    int num_ligacoes = 0, num_carreiras = 0, num_paragens = 0;
    /* O comando 'q' termina o programa. */
    while((comando = getchar()) != 'q'){
        if(fgets(linha, LINHA_MAX, stdin)!= NULL){ 
            argumentos = conta_argumentos(linha);
            /* O comando 'c' lista, mostra e adiciona carreiras. */
            if(comando == 'c'){
                if(argumentos == 0)
                    lista_carreiras(carreiras, num_carreiras, ligacoes,
                                   num_ligacoes);
                else if(argumentos == 1){
                    /* Só se pretende mostrar carreiras que existam. */
                    if(verifica_carreira(linha, num_carreiras, num_paragens,
                                        carreiras, paragens, ligacoes))
                        mostra_carreira(linha,paragens,num_paragens,carreiras,
                                        num_carreiras, ligacoes, num_ligacoes);
                    else {   
                        if(cria_carreira(linha, paragens, num_paragens, 
                                carreiras, num_carreiras,ligacoes)!= NULL){
                            num_carreiras++; 
                            carreiras_aux = (Carreira*) realloc(carreiras,
                                    sizeof(Carreira)*(num_carreiras + 1));
                            verifica_memoria(paragens, num_paragens, carreiras,
                                    num_carreiras, ligacoes, carreiras_aux);
                            carreiras = carreiras_aux;
                            carreiras_aux = NULL;  
                        }
                    }
                }
                else if(argumentos == 2)
                    mostra_carreira_inversa(linha, paragens, num_paragens, 
                                            carreiras, num_carreiras, 
                                            ligacoes, num_ligacoes);
            }
            /* O comando 'p' lista, mostra e adiciona paragens. */
            else if(comando == 'p'){
                if(argumentos == 0)
                    lista_paragens(paragens, num_paragens, carreiras,
                                  num_carreiras, ligacoes, num_ligacoes);
                else if(argumentos == 1)
                    mostra_paragem(linha, paragens, num_paragens, carreiras,
                                num_carreiras, ligacoes);
                else if(argumentos == 3){
                    if(cria_paragem(linha, paragens, num_paragens, carreiras,
                                    num_carreiras, ligacoes) != NULL){
                        num_paragens++;
                        paragens_aux = (Paragem*) realloc(paragens,
                                    sizeof(Paragem)*(num_paragens + 1)); 
                        verifica_memoria(paragens, num_paragens, carreiras,
                                    num_carreiras, ligacoes, paragens_aux);
                        paragens = paragens_aux;
                        paragens_aux = NULL;
                    }
                }
            }
            /* O comando 'l' adiciona ligações. */
            else if(comando == 'l' && argumentos == 5){
                if(adiciona_ligacoes(linha, paragens, num_paragens, carreiras,
                                 num_carreiras, ligacoes, num_ligacoes)){
                    num_ligacoes++;
                    ligacoes_aux = (Ligacao*) realloc(ligacoes,
                                sizeof(Ligacao)* (num_ligacoes + 1)); 
                    verifica_memoria(paragens, num_paragens, carreiras, 
                                    num_carreiras, ligacoes, ligacoes_aux);
                    ligacoes = ligacoes_aux;
                    ligacoes_aux = NULL;          
                }
            }
            /* O comando 'i' lista os nós de interligação. */
            else if(comando == 'i' && argumentos == 0)
                lista_intersecoes(paragens, num_paragens, carreiras, 
                                num_carreiras, ligacoes, num_ligacoes);
            /* O comando 'r' remove carreiras do sistema. */                    
            else if(comando == 'r' && argumentos == 1){
                i = remove_carreira(linha, paragens, num_paragens, carreiras,
                                num_carreiras, ligacoes, num_ligacoes);
                if(i != 0){
                    num_ligacoes = num_ligacoes - i + 1;
                    num_carreiras--;
                }
            }
            /* O comando 'e' elimina paragens do sistema. */
            else if(comando == 'e' && argumentos == 1){
                i = elimina_paragem(linha, paragens, num_paragens,ligacoes,
                                    num_ligacoes,carreiras,num_carreiras);
                if(i != 0){
                    num_ligacoes = num_ligacoes - i + 1;
                    num_paragens--;
                }
            }
            /* O comando 'a' apaga todos os dados do sistema. */
            else if(comando == 'a' && argumentos == 0){
                for(i = 0; i < num_paragens; i++){
                    free(paragens[i].nome);
                    paragens[i].nome = NULL;
                }
                free(paragens);
                paragens = (Paragem*)malloc(sizeof(Paragem));

                for(i = 0; i < num_carreiras; i++){
                    free(carreiras[i].nome);
                    carreiras[i].nome = NULL;
                }
                free(carreiras);
                carreiras = (Carreira*)malloc(sizeof(Carreira));

                free(ligacoes);
                ligacoes = (Ligacao*)malloc(sizeof(Ligacao));
                num_paragens = num_carreiras = num_ligacoes = 0;
            }      
        }
    }
    /* No fim da execução dos comandos a memória dos vetores é libertada. */
    limpa_vetores(paragens, num_paragens, carreiras, num_carreiras, ligacoes);
    return 0;
}

