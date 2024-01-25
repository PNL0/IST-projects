; *********************************************************************************
;  Projeto IAC 2022/2023
;  Grupo 05  
;
;  Autores:
;  Duarte Laia   - 106876
;  Pedro Leal    - 106154
;  Eduardo Silva - 106929
; *********************************************************************************

; *********************************************************************************
; * Constantes
; *********************************************************************************
DISPLAYS                EQU 0A000H              ; endereço dos displays (periférico POUT-1)
TEC_LIN				    EQU 0C000H	            ; endereço das linhas do teclado (periférico POUT-2)
TEC_COL				    EQU 0E000H	            ; endereço das colunas do teclado (periférico PIN)
LINHAOFFSET             EQU 10000b              ; linha com offset, para não ter que dar fix no fim
LINHATESTE				EQU 1000H				; linha aleatoria para gerar numeros aleatorios
MASCARA				    EQU 0FH		            ; para isolar os 4 bits de menor peso, ao ler as colunas do teclado
TECLA_4					EQU 4
TECLA_D                 EQU 0DH
TECLA_E                 EQU 0EH
TECLA_C                 EQU 0CH

TAMANHO_PILHA			EQU 100H				; tamanho de cada pilha, em words

VALOR_ENERGIA_INICIO    EQU 64H                 ; valor inicial da energia
DECREMENTO_ENERGIA_PERIODICO  EQU -3			; valor de energia a decrementar periodicamente
ENERGIA_ASTEROIDE_MINERAVEL	EQU 25				; valor de energia a aumentar quando um asteroide mineravel é atingido

HIGH_SCORE_INICIO				EQU 0			; high score inicial

COMANDOS				EQU	6000H			    ; endereço de base dos comandos do MediaCenter

DEFINE_LINHA    		EQU COMANDOS + 0AH		; endereço do comando para definir a linha
DEFINE_COLUNA   		EQU COMANDOS + 0CH		; endereço do comando para definir a coluna
DEFINE_PIXEL    		EQU COMANDOS + 12H		; endereço do comando para escrever um pixel
APAGA_AVISO     		EQU COMANDOS + 40H	    ; endereço do comando para apagar o aviso de nenhum cenário selecionado
APAGA_ECRÃ	 		    EQU COMANDOS + 02H	    ; endereço do comando para apagar todos os pixels já desenhados
SELECIONA_CENARIO_FUNDO EQU COMANDOS + 42H		; endereço do comando para selecionar uma imagem de fundo
APAGA_CENARIO_FUNDO		EQU COMANDOS + 40H		; endereço do comando para apagar a imagem de fundo
SELECIONA_CENARIO_FRONTAL EQU COMANDOS + 46H	; endereço do comando para selecionar uma imagem frontal
APAGA_CENARIO_FRONTAL	EQU COMANDOS + 44H		; endereço do comando para apagar a imagem frontal
TOCA_SOM				EQU COMANDOS + 5AH		; endereço do comando para tocar um som
APAGA_PIXEIS        	EQU COMANDOS + 02H      ; endereço do comando para apagar os pixeis de todos os ecras
MOSTRA_ECRA             EQU COMANDOS + 06H      ; endereço do comando para mostrar o ecrã
SELECIONA_ECRA          EQU COMANDOS + 04H      ; endereço do comando para selecionar um ecrã
ESCONDE_ECRA            EQU COMANDOS + 08H      ; endereço do comando para esconder um ecrã

ECRA_JOGO				EQU 0
ECRA_PARADO				EQU 1
ECRA_MENU_INICIO		EQU 2
IMAGEM_FRONTAL_PAUSA	EQU 3
IMAGEM_SEM_ENERGIA		EQU 4
IMAGEM_NAVE_DESTRUIDA	EQU 5
IMAGEM_PAROU_FORCADO	EQU 6

CAMADA_PADRAO_NAVE       EQU 3
CAMADA_SONDA      		 EQU 4
CAMADA_BASE_ASTEROIDES   EQU 5

SOM_SONDA				EQU 0
SOM_NAVE_INICIAR		EQU 1
SOM_ACABA_JOGO			EQU 2
SOM_PAUSA				EQU 3
SOM_METEORO				EQU 4
SOM_GOOD_JOB			EQU 5

MIN_COLUNA		        EQU    0                ; número da coluna mais à esquerda que o objeto pode ocupar
MAX_COLUNA		        EQU   63                ; número da coluna mais à direita que o objeto pode ocupar
MIN_LINHA		        EQU    0                ; número da linha mais em cima que o objeto pode ocupar
MAX_LINHA		        EQU   31                ; número da linha mais em baixo que o objeto pode ocupar

LARGURA_NAVE		    EQU  15		            ; largura da nave
ALTURA_NAVE			    EQU   8		            ; altura da nave
LINHA_NAVE            	EQU  MAX_LINHA+1-ALTURA_NAVE ; linha da nave
COLUNA_NAVE			    EQU  25                 ; coluna da nave
OFFSET_COLUNA_NAVE_COLISAO EQU 2
LARGURA_PADRAO_NAVE		EQU  7					; largura do padrão da nave
ALTURA_PADRAO_NAVE		EQU  2					; altura do padrão da nave
LINHA_PADRAO_NAVE		EQU  LINHA_NAVE+3		; linha do padrão da nave
COLUNA_PADRAO_NAVE		EQU	 COLUNA_NAVE+4		; coluna do padrão da nave
NUM_PADROES_NAVE		EQU  4					; numero de padroes da nave

NUMERO_SONDAS			EQU 3					; numero de sondas
LINHA_SONDA_MEIO		EQU LINHA_NAVE - 1      ; a sonda comeca em cima da nave
COLUNA_SONDA_MEIO	    EQU COLUNA_NAVE + (LARGURA_NAVE/2)  ; a sonda comeca a meio da nave
LINHA_SONDA_ESQUERDA	EQU LINHA_NAVE + 2
COLUNA_SONDA_ESQUERDA	EQU COLUNA_NAVE + 2
LINHA_SONDA_DIREITA		EQU LINHA_NAVE + 2
COLUNA_SONDA_DIREITA	EQU COLUNA_NAVE + LARGURA_NAVE - 3
ALTURA_SONDA		    EQU 1				    ; altura da sonda (é preciso definir para utilizar a rotina genérica de desenha_objeto)
LARGURA_SONDA		    EQU 1				    ; largura da sonda 
ALCANCE_MAXIMO_SONDA	EQU MAX_LINHA-ALTURA_NAVE-12
LINHA_POSICAO_SEGURA_SONDA	EQU MAX_LINHA		; linha para uma posição de reset da sonda para não ocorrer colisões inesperadas
COLUNA_POSICAO_SEGURA_SONDA	EQU MAX_COLUNA/2	; coluna para uma posição de reset da sonda para não ocorrer colisões inesperadas

LARGURA_ASTEROIDE       EQU  5                  ; largura do asteroide
ALTURA_ASTEROIDE        EQU  5                  ; altura do asteroide
NUMERO_ASTEROIDES		EQU 4					; numero de asteroides 
LINHA_ASTEROIDE		    EQU  MIN_LINHA + 1    ; considerando o ponto inicial do asteroide o canto superior esquerdo
COLUNA_ASTEROIDE_ESQUERDA EQU  MIN_COLUNA + 1   ; posicionando 1 pixel antes do limite 
COLUNA_ASTEROIDE_MEIO 	EQU  (MIN_COLUNA+MAX_COLUNA)/2-(LARGURA_ASTEROIDE/2)   ; posicionando no meio da tela
COLUNA_ASTEROIDE_DIREITA EQU  MAX_COLUNA - LARGURA_ASTEROIDE   ; posicionando 1 pixel antes do limite
NUMERO_ANIMACOES_ASTEROIDE EQU 2

; Cores asteroide
CINZENTO_ESCURO_AST		EQU  0F555H
CINZENTO_CLARO_AST		EQU  0F777H
VERDE_ESCURO			EQU  0F161H
VERDE_CLARO				EQU  0F181H
 
; Cores nave
ROXO_BORDA			    EQU  0F508H
ROXO_SOMBRA			    EQU  0F96DH
ROXO_CLARO_SOMBRA	    EQU  0F95DH
ROXO_CLARO			    EQU  0F96DH
AZUL_SOMBRA			    EQU  0F68FH
AZUL_CLARO_SOMBRA	    EQU  0F79FH
AZUL_CLARO			    EQU  0F8AFH
AZUL_DESFOCADO_FORA 	EQU  018AFH
AZUL_DESFOCADO_DENTRO_1 EQU  028AFH
AZUL_DESFOCADO_DENTRO_2 EQU  038AFH
BRANCO					EQU  0FFFFH
VERMELHO				EQU  0FF00H
AMARELO		   			EQU  0FFD0H
LARANJA					EQU  0FF60H

; Estado do jogo
ATIVO					EQU 0
PAUSADO 				EQU 1
PARADO					EQU 2
INICIO   				EQU 3

; Razões para jogo parado
; nota: As razões correspondem aos overlays a apresentar
;		na tela de jogo parado para fácil acesso
NAVE_EXPLODIU			EQU IMAGEM_NAVE_DESTRUIDA		
ACABOU_ENERGIA			EQU IMAGEM_SEM_ENERGIA
ACABOU_FORCADO			EQU IMAGEM_PAROU_FORCADO


; *********************************************************************************
; * Dados 
; *********************************************************************************
PLACE       1000H

	STACK TAMANHO_PILHA			                                 
SP_inicial_principal:				           ; espaço reservado para o programa principal

	STACK TAMANHO_PILHA
SP_inicial_teclado: 						   ; espaço reservado para o processo do teclado

	STACK TAMANHO_PILHA
SP_inicial_padrao: 						   	   ; espaço reservado para o processo do padrao da nave

	STACK TAMANHO_PILHA*NUMERO_SONDAS
SP_inicial_sondas: 						   	   ; espaço reservado para o processo de comportamento de sondas

	STACK TAMANHO_PILHA*NUMERO_ASTEROIDES
SP_inicial_asteroides: 						   ; espaço reservado para o processo de comportamento de asteroides

	STACK TAMANHO_PILHA
SP_inicial_energia: 						   ; espaço reservado para o processo de decrescimo de energia

	STACK TAMANHO_PILHA
SP_inicial_pausado: 						   ; espaço reservado para o processo de toggle da pausa

	STACK TAMANHO_PILHA
SP_inicial_parado: 				  	  		   ; espaço reservado para o processo do jogo parado quando o jogo está parado

	STACK TAMANHO_PILHA
SP_inicial_terminou: 				  		   ; espaço reservado para o processo do jogo parado quando o jogador termina voluntariamente o jogo

	STACK TAMANHO_PILHA
SP_inicial_deteta_colisoes: 				   ; espaço reservado para o processo de verificação de colisões


tab:
	WORD move_asteroides					   ; rotina de atendimento da interrupção do movimento de asteroides
	WORD move_sondas						   ; rotina de atendimento da interrupção do movimento de sondas
	WORD decresce_energia_periodicamente	   ; rotina de atendimento de alteracao da energia
	WORD desbloqueia_altera_padrao_nave		   ; rotina de desbloqueio de alteracao do padrao da nave

; Nave
DEF_NAVE:
	WORD LARGURA_NAVE
	WORD ALTURA_NAVE
	WORD 0, AZUL_DESFOCADO_FORA, AZUL_DESFOCADO_FORA, AZUL_DESFOCADO_FORA, AZUL_DESFOCADO_FORA, AZUL_DESFOCADO_FORA, ROXO_BORDA, ROXO_BORDA, ROXO_BORDA, AZUL_DESFOCADO_FORA, AZUL_DESFOCADO_FORA, AZUL_DESFOCADO_FORA, AZUL_DESFOCADO_FORA, AZUL_DESFOCADO_FORA, 0
	WORD AZUL_DESFOCADO_FORA, AZUL_DESFOCADO_DENTRO_1, AZUL_DESFOCADO_DENTRO_1, AZUL_DESFOCADO_DENTRO_1, AZUL_DESFOCADO_DENTRO_2, ROXO_BORDA, AZUL_CLARO, AZUL_CLARO_SOMBRA, AZUL_CLARO, ROXO_BORDA, AZUL_DESFOCADO_DENTRO_2, AZUL_DESFOCADO_DENTRO_1, AZUL_DESFOCADO_DENTRO_1, AZUL_DESFOCADO_DENTRO_1, AZUL_DESFOCADO_FORA
	WORD AZUL_DESFOCADO_FORA, AZUL_DESFOCADO_DENTRO_1, AZUL_DESFOCADO_DENTRO_2, ROXO_BORDA, ROXO_BORDA, AZUL_CLARO_SOMBRA, AZUL_CLARO, AZUL_CLARO_SOMBRA, AZUL_CLARO_SOMBRA, AZUL_SOMBRA, ROXO_BORDA, ROXO_BORDA, AZUL_DESFOCADO_DENTRO_2, AZUL_DESFOCADO_DENTRO_1, AZUL_DESFOCADO_FORA
	WORD AZUL_DESFOCADO_FORA, ROXO_BORDA, ROXO_BORDA, ROXO_BORDA, AZUL_CLARO_SOMBRA, AZUL_CLARO, AZUL_CLARO_SOMBRA, AZUL_CLARO, AZUL_SOMBRA, AZUL_CLARO, AZUL_CLARO_SOMBRA, ROXO_BORDA, ROXO_BORDA, ROXO_BORDA, AZUL_DESFOCADO_FORA
	WORD ROXO_BORDA, ROXO_CLARO, ROXO_CLARO_SOMBRA, ROXO_CLARO, ROXO_BORDA, AZUL_CLARO_SOMBRA, AZUL_SOMBRA, AZUL_CLARO_SOMBRA, AZUL_SOMBRA, AZUL_CLARO_SOMBRA, ROXO_BORDA, ROXO_SOMBRA, ROXO_CLARO_SOMBRA, ROXO_SOMBRA, ROXO_BORDA
	WORD ROXO_BORDA, ROXO_CLARO, ROXO_CLARO, ROXO_CLARO_SOMBRA, ROXO_CLARO_SOMBRA, ROXO_BORDA, ROXO_BORDA, ROXO_BORDA, ROXO_BORDA, ROXO_BORDA, ROXO_CLARO_SOMBRA, ROXO_CLARO_SOMBRA, ROXO_SOMBRA, ROXO_SOMBRA, ROXO_BORDA
	WORD AZUL_DESFOCADO_FORA, ROXO_BORDA, ROXO_CLARO_SOMBRA, ROXO_CLARO_SOMBRA, ROXO_CLARO, ROXO_CLARO_SOMBRA, ROXO_CLARO_SOMBRA, ROXO_CLARO, ROXO_CLARO_SOMBRA, ROXO_CLARO_SOMBRA, ROXO_CLARO_SOMBRA, ROXO_SOMBRA, ROXO_CLARO_SOMBRA, ROXO_BORDA, AZUL_DESFOCADO_FORA
	WORD 0, AZUL_DESFOCADO_FORA, ROXO_BORDA, ROXO_BORDA, ROXO_CLARO_SOMBRA, ROXO_CLARO, ROXO_CLARO, ROXO_CLARO_SOMBRA, ROXO_CLARO_SOMBRA, ROXO_SOMBRA, ROXO_SOMBRA, ROXO_BORDA, ROXO_BORDA, AZUL_DESFOCADO_FORA, 0
	
padroes_nave:								   ; tabela de todos os padrões da nave
	WORD padrao_nave_0
	WORD padrao_nave_1
	WORD padrao_nave_2
	WORD padrao_nave_3

padrao_nave_0:
	WORD LARGURA_PADRAO_NAVE
	WORD ALTURA_PADRAO_NAVE
	WORD AZUL_CLARO_SOMBRA, AZUL_CLARO, AZUL_CLARO_SOMBRA, AZUL_CLARO, AZUL_SOMBRA, AZUL_CLARO, AZUL_CLARO_SOMBRA
	WORD ROXO_BORDA, AZUL_CLARO_SOMBRA, AZUL_SOMBRA, AZUL_CLARO, AZUL_SOMBRA, AZUL_CLARO_SOMBRA, ROXO_BORDA

padrao_nave_1:
	WORD LARGURA_PADRAO_NAVE
	WORD ALTURA_PADRAO_NAVE
	WORD AZUL_CLARO_SOMBRA, AZUL_CLARO, AZUL_CLARO_SOMBRA, AZUL_CLARO, AZUL_SOMBRA, AZUL_CLARO, AZUL_CLARO_SOMBRA
	WORD ROXO_BORDA, VERMELHO, AZUL_SOMBRA, AZUL_CLARO, AZUL_SOMBRA, VERMELHO, ROXO_BORDA

padrao_nave_2:
	WORD LARGURA_PADRAO_NAVE
	WORD ALTURA_PADRAO_NAVE
	WORD AZUL_CLARO_SOMBRA, AZUL_CLARO, AZUL_CLARO_SOMBRA, AZUL_CLARO, AZUL_SOMBRA, AZUL_CLARO, AZUL_CLARO_SOMBRA
	WORD ROXO_BORDA, AZUL_CLARO_SOMBRA, LARANJA, AZUL_CLARO, LARANJA, AZUL_CLARO_SOMBRA, ROXO_BORDA

padrao_nave_3:
	WORD LARGURA_PADRAO_NAVE
	WORD ALTURA_PADRAO_NAVE
	WORD AZUL_CLARO_SOMBRA, AZUL_CLARO, AZUL_CLARO_SOMBRA, AZUL_CLARO, AZUL_SOMBRA, AZUL_CLARO, AZUL_CLARO_SOMBRA
	WORD ROXO_BORDA, AZUL_CLARO_SOMBRA, AZUL_SOMBRA, AMARELO, AZUL_SOMBRA, AZUL_CLARO_SOMBRA, ROXO_BORDA

; Sondas
; NOTA: O identificador das sondas corresponde á tecla que a desbloqueia
DEF_SONDA:
	WORD LARGURA_SONDA
	WORD ALTURA_SONDA
	WORD AMARELO
	
posicao_sondas_inicio:
	WORD LINHA_SONDA_ESQUERDA                 
	WORD COLUNA_SONDA_ESQUERDA				  
	WORD LINHA_SONDA_MEIO					   
	WORD COLUNA_SONDA_MEIO					   
	WORD LINHA_SONDA_DIREITA				   
	WORD COLUNA_SONDA_DIREITA				   
	
posicao_sondas_atual:					  	   ; Guarda as posicoes das sondas nos mesmos moldes 
	TABLE NUMERO_SONDAS*2				       ; de 'posicao_sondas_inicio'
	
; Asteroides
animacao_asteroides_nao_mineravel:		    	; tabela de padrões dos asteroides nao mineraveis
	WORD DEF_ASTEROIDE_NAO_MINERAVEL
	WORD asteroide_nao_mineravel_aimacao_1
	WORD asteroide_nao_mineravel_aimacao_2
	
DEF_ASTEROIDE_NAO_MINERAVEL:
	WORD LARGURA_ASTEROIDE
	WORD ALTURA_ASTEROIDE
	WORD 0,CINZENTO_ESCURO_AST,CINZENTO_CLARO_AST,CINZENTO_ESCURO_AST,0
	WORD CINZENTO_ESCURO_AST,CINZENTO_ESCURO_AST,CINZENTO_ESCURO_AST,CINZENTO_CLARO_AST,CINZENTO_ESCURO_AST
	WORD CINZENTO_CLARO_AST,CINZENTO_ESCURO_AST,CINZENTO_CLARO_AST,CINZENTO_ESCURO_AST,CINZENTO_ESCURO_AST
	WORD CINZENTO_ESCURO_AST,CINZENTO_ESCURO_AST,CINZENTO_ESCURO_AST,CINZENTO_ESCURO_AST,CINZENTO_CLARO_AST
	WORD 0,CINZENTO_CLARO_AST,CINZENTO_ESCURO_AST,CINZENTO_CLARO_AST,0

asteroide_nao_mineravel_aimacao_1:
	WORD LARGURA_ASTEROIDE
	WORD ALTURA_ASTEROIDE
	WORD 0,CINZENTO_ESCURO_AST,0,CINZENTO_ESCURO_AST,0
	WORD CINZENTO_ESCURO_AST,CINZENTO_ESCURO_AST,0,CINZENTO_CLARO_AST,CINZENTO_ESCURO_AST
	WORD 0,CINZENTO_ESCURO_AST,CINZENTO_CLARO_AST,0,CINZENTO_ESCURO_AST
	WORD CINZENTO_ESCURO_AST,CINZENTO_ESCURO_AST,CINZENTO_ESCURO_AST,0,CINZENTO_CLARO_AST
	WORD 0,CINZENTO_CLARO_AST,CINZENTO_ESCURO_AST,CINZENTO_CLARO_AST,0

asteroide_nao_mineravel_aimacao_2:
	WORD LARGURA_ASTEROIDE
	WORD ALTURA_ASTEROIDE
	WORD 0,CINZENTO_ESCURO_AST,0,0,0
	WORD 0,0,0,CINZENTO_CLARO_AST,0
	WORD 0,0,0,0,CINZENTO_ESCURO_AST
	WORD 0,CINZENTO_ESCURO_AST,0,0,0
	WORD 0,CINZENTO_CLARO_AST,0,CINZENTO_CLARO_AST,0

animacao_asteroides_mineravel:		    		; tabela de padrões dos asteroides mineraveis
	WORD DEF_ASTEROIDE_MINERAVEL
	WORD asteroide_mineravel_aimacao_1
	WORD asteroide_mineravel_aimacao_2

DEF_ASTEROIDE_MINERAVEL:
	WORD LARGURA_ASTEROIDE
	WORD ALTURA_ASTEROIDE
	WORD 0,VERDE_ESCURO,VERDE_CLARO,VERDE_ESCURO,0
	WORD VERDE_CLARO,VERDE_CLARO,VERDE_ESCURO,VERDE_CLARO,VERDE_ESCURO
	WORD VERDE_CLARO,VERDE_ESCURO,VERDE_CLARO,VERDE_CLARO,VERDE_CLARO
	WORD VERDE_ESCURO,VERDE_CLARO,VERDE_CLARO,VERDE_ESCURO,VERDE_ESCURO
	WORD 0,VERDE_CLARO,VERDE_ESCURO,VERDE_CLARO,0

asteroide_mineravel_aimacao_1:
	WORD LARGURA_ASTEROIDE
	WORD ALTURA_ASTEROIDE
	WORD 0,0,0,0,0
	WORD 0,0,VERDE_ESCURO,0,0
	WORD 0,VERDE_ESCURO,VERDE_CLARO,VERDE_CLARO,0
	WORD 0,0,VERDE_CLARO,0,0
	WORD 0,0,0,0,0

asteroide_mineravel_aimacao_2:
	WORD LARGURA_ASTEROIDE
	WORD ALTURA_ASTEROIDE
	WORD 0,0,0,0,0
	WORD 0,0,0,0,0
	WORD 0,0,VERDE_CLARO,0,0
	WORD 0,0,0,0,0
	WORD 0,0,0,0,0
	
direcoes_colunas_aleatorias:					; Conjuntos (coluna - direção horizontal) que um asteroide pode ter 
	WORD COLUNA_ASTEROIDE_ESQUERDA
	WORD 1
	WORD COLUNA_ASTEROIDE_MEIO
	WORD -1
	WORD COLUNA_ASTEROIDE_MEIO
	WORD 0
	WORD COLUNA_ASTEROIDE_MEIO
	WORD 1
	WORD COLUNA_ASTEROIDE_DIREITA
	WORD -1

posicao_asteroides_atual:					   ; Guarda as posicoes dos asteroides em forma de conjuntos 
	TABLE NUMERO_ASTEROIDES*2				   ; de words (linha - coluna)

; Variáveis de controlo de spawn de asteroides
direcoes_disponiveis:
	WORD 1                                    ; ENABLE do percurso da esquerda para o meio
	WORD 1                                    ; ENABLE do percurso do meio para a esquerda
	WORD 1                                    ; ENABLE do percurso do meio para baixo
	WORD 1                                    ; ENABLE do percurso do meio para a direita
	WORD 1                                    ; ENABLE do percurso da direita para o meio

; Energia
valor_energia:
	WORD VALOR_ENERGIA_INICIO

; Controlo do jogo
estado_jogo:
	WORD ATIVO								   ; Estado do jogo (assim que sair do menu inicial, fica ativo)

razao_parado:								   ; Motivo para o jogo estar parado:
	WORD 0									   ; nave_explodiu, acabou_energia, acabou_forcado

toggle_pausa_jogo:							   ; Evento que faz com que o jogo saia e entra em modo de pausa
	LOCK PARADO								   ; guarda o ultimo estado em que o jogo estava (parado, pausado)

; Tabela de bits de verificação de colisão com sondas
colisao_sonda:
	WORD 0									   ; Colisão com sonda 0
	WORD 0									   ; Colisão com sonda 1
	WORD 0									   ; Colisão com sonda 2
	
; Tabela de bits de verificação de colisão com asteroides
colisao_asteroide:
	WORD 0									   ; Colisão com asteroide 0
	WORD 0									   ; Colisão com asteroide 1
	WORD 0									   ; Colisão com asteroide 2
	WORD 0									   ; Colisão com asteroide 3

eventos_interrupcoes:
	LOCK 0									   ; Evento do movimento de asteroides
	LOCK 0									   ; Evento do movimento de sondas
	LOCK 0									   ; Evento de decréscimo de energia
	LOCK 0									   ; Evento da alteraçao do padrão da nave
	
tecla_carregada:
	LOCK 0                                     ; LOCK para o teclado comunicar aos restantes processos que tecla detetou

score_atual:
	WORD HIGH_SCORE_INICIO
	
high_score:
	WORD HIGH_SCORE_INICIO

; *********************************************************************************
; * Código
; *********************************************************************************
PLACE   0                                     ; o código tem de começar em 0000H

	MOV  SP, SP_inicial_principal		      ; inicializa SP principal
	
	MOV BTE, tab							  ; inicializa BTE (registo de Base da Tabela de Exceções)
	 
	; Setup da tela inicial 
	MOV  [APAGA_AVISO], R1	                  ; apaga o aviso de nenhum cenário selecionado (o valor de R1 não é relevante)
    MOV  [APAGA_ECRÃ], R1	                  ; apaga todos os pixeis já desenhados (o valor de R1 não é relevante)
	MOV	 R0, ECRA_MENU_INICIO 				  ; cenário de tela de inicio de jogo                    
	MOV [SELECIONA_CENARIO_FUNDO],R0

	; Iniciar teclado
	CALL teclado
	
	MOV [APAGA_CENARIO_FRONTAL], R0          ; apaga qualquer cenário frontal
	
	; Reseta o high score
	MOV R0, HIGH_SCORE_INICIO
	MOV [high_score], R0
	
	; Resetar o valor da energia
	MOV R0, valor_energia                     ; Inicializa endereços e valores iniciais
	MOV R1, 0     
	MOV [R0], R1					          ; Altera o valor na memória
	CALL display_energia			          ; Atualiza o valor do display
	
menu_inicial:
	;cenário para começar o jogo
	MOV R0, [tecla_carregada]
	MOV R1, TECLA_C
	CMP R0 , R1
	JNZ menu_inicial
	
inicio_jogo:
	
	EI0									 	  ; Permite interrupcao do movimento dos asteroides
	EI1										  ; Permite interrupcao do movimento das sondas
	EI2										  ; Permite interrupcao de decremento de energia
	EI3										  ; Permite interrupcao de alteracao do padrao da nave
	EI
	
	; Inicia o estado do jogo e reseta valores em memória
	MOV R1, ATIVO
	MOV [estado_jogo], R1 

	CALL reseta_jogo						; Resetar o jogo
	
	; Processos relativos ao jogo
	CALL altera_padrao_nave
	CALL processo_descresce_energia
	CALL controlo_jogo_pausado
	CALL controlo_jogo_parado
	CALL controlo_jogo_terminou
	
	; Sondas
	MOV R11, NUMERO_SONDAS
	inicializa_sondas:
		SUB R11, 1							  ; Proxima sonda
		CALL comportamento_sondas			  ; Inicializa o processo para cada sonda
		CMP R11, 0
		JNZ inicializa_sondas
	
	; Asteroides
	MOV R11, NUMERO_ASTEROIDES
	inicializa_asteroides:
		SUB R11, 1							  ; Proximo asteroide
		CALL comportamento_asteroides		  ; Inicializa o processo para cada asteroide
		CMP R11, 0
		JNZ inicializa_asteroides
	
	CALL deteta_colisoes					

jogo_iniciado:								  ; Loop do jogo
	YIELD
	JMP jogo_iniciado

; **********************************************************************
;
; Processo:
;
; CONTROLO_JOGO_PAUSADO - Controla o estado do jogo durante o mesmo
;	
; **********************************************************************
PROCESS SP_inicial_pausado

controlo_jogo_pausado:
	MOV R0, [tecla_carregada]				 ; Verificar se a tecla de pausa foi pressionada
	MOV R1, TECLA_D
	CMP R0, R1
	JNZ controlo_jogo_pausado				 ; Se não for, ficar em ciclo
	
	MOV R1, [estado_jogo]
	CMP R1, PARADO
	JZ controlo_jogo_pausado				 ; Se o jogo estiver parado, não fazer nada
	
	MOV R1, [estado_jogo]
	CMP R1, ATIVO
	JNZ ativar_jogo							 ; Se o jogo estiver pausado, ativar
	
	; Se o jogo estiver ativo, pausar
	MOV R0, IMAGEM_FRONTAL_PAUSA			 ; Mostra overlay de pausa
	MOV [SELECIONA_CENARIO_FRONTAL], R0
	
	MOV R0, SOM_PAUSA						 ; Toca o som de pausa
	MOV [TOCA_SOM], R0
	
	MOV R0, PAUSADO
	MOV [estado_jogo], R0					 ; Atualizar o estado do jogo
	JMP controlo_jogo_pausado				 ; Não atualizamos o ultimo estado porque seria ativo -> irrelevante para comparações futuras
	
	ativar_jogo:
		MOV R0, IMAGEM_FRONTAL_PAUSA		 ; Apaga overlay de pausa
		MOV [APAGA_CENARIO_FRONTAL], R0
		
		MOV R0, SOM_PAUSA					 ; Toca o som de pausa
		MOV [TOCA_SOM], R0
		
		MOV R0, ATIVO
		MOV R1, PAUSADO
		MOV [estado_jogo], R0				 ; Atualizar o estado do jogo
		MOV [toggle_pausa_jogo], R1			 ; Atualizar o último estado do jogo
	
	JMP controlo_jogo_pausado

; **********************************************************************
;
; Processo:
;
; CONTROLO_JOGO_PARADO - Controla o estado do jogo após o jogo estar parado
;	
; **********************************************************************
PROCESS SP_inicial_parado

controlo_jogo_parado:
	YIELD
	MOV R0, [estado_jogo]
	MOV R1, PARADO
	CMP R0, R1
	JNZ controlo_jogo_parado				 ; Verificar se o jogo está parado
	
	DI
	
	; Atualização do high score (se necessário)
	MOV R0, [score_atual]
	MOV R1, [high_score]
	CMP R0, R1
	JLT continua_jogo_parado
	MOV [high_score], R0
	
	
	continua_jogo_parado:
	
	; Toca o som do jogo parado
	MOV R0, SOM_ACABA_JOGO
	MOV [TOCA_SOM], R0
	
	; Mostrar o cenário do jogo parado
	MOV  R2,ECRA_PARADO
	MOV  [APAGA_ECRÃ], R1	                  ; apaga todos os pixeis já desenhados (o valor de R1 não é relevante)
	MOV  [SELECIONA_CENARIO_FUNDO],R2
	
	; Mostrar o overlay da razão pelo jogo estar parado
	MOV R0, [razao_parado]				
	MOV [SELECIONA_CENARIO_FRONTAL], R0
	
	ciclo_jogo_parado:
		MOV R0, [tecla_carregada]			 ; Verificar se a tecla de reiniciar foi pressionada
		MOV R1, TECLA_C
		CMP R0, R1
		JZ volta_jogo						 ; Reiniciar o jogo
		MOV R1, TECLA_4						 ; Verifica se é para mostrar o high score
		CMP R0, R1
		JNZ ciclo_jogo_parado
	
	; Mostra o high score
	; São utilizadas variáveis relativas à energia, uma vez
	; que as rotinas estão implementadas para tal 
	MOV R2, [high_score]
	MOV R0, valor_energia                     ; Endereço da energia (valor que é mostrado no display)  
	MOV [R0], R2					          ; Altera o valor na memória
	CALL display_energia			          ; Atualiza o valor do display
	
	JMP ciclo_jogo_parado
	
	volta_jogo:
	; Apagar o overlay da razão pelo jogo estar parado
	MOV R0, [razao_parado]				
	MOV [APAGA_CENARIO_FRONTAL], R0
	
	; Reiniciar o jogo
	MOV R0, ATIVO
	MOV R1, PARADO
	MOV [estado_jogo], R0				 	; Atualizar o estado do jogo
	MOV [toggle_pausa_jogo], R1			 	; Atualizar o último estado do jogo
	
	CALL reseta_jogo
	
	JMP controlo_jogo_parado

; **********************************************************************
;
; Processo:
;
; CONTROLO_JOGO_TERMINOU - Verifica se o jogador terminou o jogo voluntariamente
;	
; **********************************************************************
PROCESS SP_inicial_terminou

controlo_jogo_terminou:

	MOV R0, [tecla_carregada]				; Verificar se a tecla de para o jogo foi pressionada
	MOV R1, TECLA_E
	CMP R0, R1
	JNZ controlo_jogo_terminou				; Se não for, ficar em ciclo
	
	MOV R0, PARADO							
	MOV [estado_jogo], R0					; Atualiza o estado do jogo
	MOV R0, ACABOU_FORCADO
	MOV [razao_parado], R0					; Atualiza a razão do jogo estar parado
	
	JMP controlo_jogo_terminou

; **********************************************************************
;
; Processo:
;
; DETETA_COLISOES - Deteta colisões entre asteroide-sonda e asteroide-nave
;		
; **********************************************************************
PROCESS SP_inicial_deteta_colisoes

deteta_colisoes:
	
	YIELD
	
	MOV R1, 0							    ; indice dos asteroides 
	MOV R2, 0								; indice das sondas 
	
	itera_asteroides:
		MOV R2, 0							; reseta o indice da sonda
		itera_sondas:
			CALL verifica_colisao_asteroide_sonda ; Verifica colisão entre asteroide R1 e sonda R2
			ADD R2, 1
			CMP R2, NUMERO_SONDAS
			JNZ itera_sondas
		CALL verifica_colisao_asteroide_nave
		ADD R1, 1
		CMP R1, NUMERO_ASTEROIDES
		JNZ itera_asteroides
	
	JMP deteta_colisoes

	
; **********************************************************************
;
; Processo:
;
; TECLADO - Faz uma leitura às teclas de uma linha do teclado e guarda o valor lido
;	
; **********************************************************************
PROCESS SP_inicial_teclado	; indicação de que a rotina que se segue é um processo,
						; com indicação do valor para inicializar o SP

teclado:					; processo que implementa o comportamento do teclado
	MOV  R4, TEC_LIN		; endereço do periférico das linhas
	MOV  R3, TEC_COL		; endereço do periférico das colunas
	MOV  R5, MASCARA		; para isolar os 4 bits de menor peso, ao ler as colunas do teclado
	MOV  R1, LINHAOFFSET	; começa na linha offset 
	
espera_tecla:				; neste ciclo espera-se até uma tecla ser premida

	WAIT				; este ciclo é potencialmente bloqueante, pelo que tem de
						; ter um ponto de fuga (aqui pode comutar para outro processo)
	 
	CALL itera_linhas       ; passa à proxima linha 
	MOVB [R4], R1			; escrever no periférico de saída (linhas)
	MOVB R0, [R3]	; ler do periférico de entrada (colunas)
	
	AND  R0, R5			; elimina bits para além dos bits 0-3
	CMP  R0, 0			; há tecla premida?
	JZ   espera_tecla		; se nenhuma tecla premida, repete
				
	CALL calcula_valor_teclado
	MOV	[tecla_carregada], R2	; informa quem estiver bloqueado neste LOCK que uma tecla foi carregada
							; (o valor escrito é o número da coluna da tecla no teclado)

ha_tecla:					; neste ciclo espera-se até NENHUMA tecla estar premida

	YIELD				; este ciclo é potencialmente bloqueante, pelo que tem de
						; ter um ponto de fuga (aqui pode comutar para outro processo)
						
    MOVB [R4], R1			; escrever no periférico de saída (linhas)
    MOVB R0, [R3]			; ler do periférico de entrada (colunas)
	AND  R0, R5			; elimina bits para além dos bits 0-3
     CMP  R0, 0			; há tecla premida?
     JNZ  ha_tecla			; se ainda houver uma tecla premida, espera até não haver

	JMP	espera_tecla		
						

; **********************************************************************
;
; Processo:
;
; ALTERA_PADRAO_NAVE: Vai iterando pelos padrões da nave
;	
; **********************************************************************
PROCESS SP_inicial_padrao

altera_padrao_nave:

	MOV R0, padroes_nave						; Endereço do primeiro padrão da nave
	MOV R3, 0									; Valor do padrão atual

	MOV R5, NUM_PADROES_NAVE*2					; numero de padrões (cada endereço ocupa 2 bytes)

	padrao_seguinte:
		
		; Verificar se o jogo está ativo
		MOV R1, [estado_jogo]
		CMP R1, ATIVO
		JNZ bloqueio_padrao_nave

		MOV R1, eventos_interrupcoes
		MOV R2, [R1+6] 							; espera pelo desbloqueio do lock
		
		; Desenha padrão
		MOV  R1, LINHA_PADRAO_NAVE	
		MOV  R2, COLUNA_PADRAO_NAVE		
		MOV R4, [R0 + R3]
		CALL desenha_objeto             		; Desenha o proximo padrao
		
		; Proxima iteração
		ADD R3, 2								; Cada endereço de padrão ocupa 2 bytes
		CMP R3, R5
		JNZ padrao_seguinte
	
	MOV R3, 0									; Quando chegar ao ultimo padrão, resetar
	JMP padrao_seguinte
	
	bloqueio_padrao_nave:
		; Verificar se o jogo está parado
		MOV R1, [estado_jogo]
		CMP R1, PARADO
		JNZ bloquear_padrao_nave
		
		MOV  [APAGA_ECRÃ], R1	                 ; apaga todos os pixeis já desenhados (o valor de R1 não é relevante)
	
	bloquear_padrao_nave:	
		MOV R1, [toggle_pausa_jogo]				; bloqueia aqui até dar toggle
		
	JMP altera_padrao_nave

; **********************************************************************
;
; Processo:
;
; COMPORTAMENTO_SONDAS - Controla o comportamento das sondas
;	
; **********************************************************************
PROCESS SP_inicial_sondas

comportamento_sondas:
	MOV	R1, TAMANHO_PILHA	; tamanho em palavras da pilha de cada processo
	MUL	R1, R11				; TAMANHO_PILHA vezes o nº da instância da sonda
	SUB	SP, R1		     	; inicializa SP da sonda atual
	
	MOV R10, CAMADA_SONDA
	
	reseta_sonda:
		; Reseta a posicao da sonda para a posição segura
		; de forma a evitar deteção de colisões indesejadas
		MOV R2, R11							; copia do indice
		SHL R2, 2							; offset para o endereço da linha da sonda
		MOV R3, posicao_sondas_atual
		MOV R4, LINHA_POSICAO_SEGURA_SONDA
		MOV [R3+R2], R4						; atualiza a linha da sonda
		ADD R2, 2							; offset para o endereço da coluna da sonda
		MOV R4, COLUNA_POSICAO_SEGURA_SONDA
		MOV [R3+R2], R4						; atualiza a coluna da sonda
		
		; Reseta o bit de deteçao de colisao
		MOV R0, R11							; copia do indice
		SHL R0, 1							; offset para o endereço da sonda na tabela de colisoes
		MOV R1, colisao_sonda
		MOV R2, 0
		MOV [R1+R0], R2
	
	espera_tecla_sonda:
		MOV R0, [tecla_carregada]			; Desbloqueia quando qualquer tecla for carregada
		CMP R0, R11							; O indice de cada sonda corresponde à tecla  
											; que é pressionada para a ativar
		JNZ espera_tecla_sonda
		
	; Verificar se o jogo está ativo, caso contrário não lançar sonda
	MOV R1, [estado_jogo]
	CMP R1, ATIVO
	JNZ espera_tecla_sonda
	
	; Toca o som da sonda
	MOV R0, SOM_SONDA
	MOV [TOCA_SOM], R0
	
	;decrescimo de energia de 5%
	MOV R2,-5
	CALL altera_energia
	
	; Reinicia a posição da sonda para a inicial
		MOV R2, R11
		MOV R3, posicao_sondas_atual
		MOV R4, posicao_sondas_inicio
		CALL reseta_objeto
	
	;REGISTOS FIXOS
	; R10 -> Camada
	; R11 -> indice da sonda
	ciclo_sonda:
	
		; Verificar se o jogo está PARADO
		MOV R1, [estado_jogo]
		CMP R1, PARADO
		JZ bloqueio_sonda
		
		; Desenha a sonda 
		MOV R3, R11		         			; Indice do objeto para argumento
		MOV R4, posicao_sondas_atual		; Tabela das posicoes das sondas
		CALL obtem_posicao_objeto		    ; Obtem a linha e a coluna da sonda
		MOV R4, DEF_SONDA					; Os registos de retorno do obtem_posicao_objeto correspondem
											; aos registos de argumento do desenha_objeto
		CALL desenha_objeto        	        ; Desenha a sonda (R10 como camada onde se vai desenhar)
		
		; Verificar se o jogo está pausado
		MOV R1, [estado_jogo]
		CMP R1, PAUSADO
		JZ bloqueio_sonda
		
		MOV R0, eventos_interrupcoes
		MOV R1, [R0+2] 						; esperar pelo lock correspondente ao movimento
											; das sondas (o valor de R1 é irrelevante)
		
		; Apagar a sonda
		MOV R3, R11		         			; Indice do objeto para argumento
		MOV R4, posicao_sondas_atual		; Tabela das posicoes das sondas
		CALL obtem_posicao_objeto		    ; Obtem a linha e a coluna da sonda
		MOV R4, DEF_SONDA					; Os registos de retorno do obtem_posicao_objeto correspondem
											; aos registos de argumento do desenha_objeto
		CALL apaga_objeto      		        ; Desenha a sonda
		
		; Testa limites
		MOV R2, ALCANCE_MAXIMO_SONDA
		CMP R1, R2							; O R1 tem a linha atual da sonda, dada pela rotina 
		JZ reseta_sonda						; 'obtem_posicao_objeto' usada anteriormente
		
		; Testa colisão com asteroide
		MOV R0, R11						    ; copia de indice
		SHL R0, 1							; offset do indice na tabela de colisoes
		MOV R2, colisao_sonda
		MOV R1, [R2+R0]						; bit para testar colisao
		CMP R1, 1
		JZ reseta_sonda						; se houve colisao, sair
		
		; Move a sonda
		MOV R1, -1							; Movimento na vertical
		MOV R0, R11							; O movimento lateral é "proporcinal" ao indice -> exemplo: movimento lateral da sonda da direita é (IndiceD - 1) = (2-1) = 1
		SUB R0, 1							; Calcular o movimento na lateral
		MOV R3, R11							; Indice da sonda	
		MOV R4, posicao_sondas_atual		; Endereço da posicao das sondas		
		CALL move_objeto	
		
		JMP ciclo_sonda
	
	bloqueio_sonda:
		MOV R1, [toggle_pausa_jogo]			; bloqueia aqui até dar toggle
		
		CMP R1, PAUSADO						; Se o ultimo estado for pausado, recomeçar no ciclo
		JZ ciclo_sonda
		
		JMP reseta_sonda					; Se o ultimo estado for parado, recomeçar de inicio

; **********************************************************************
;
; Processo:
;
; COMPORTAMENTO_ASTEROIDES - Controla o comportamento dos asteroides
;	
; **********************************************************************
PROCESS SP_inicial_asteroides

comportamento_asteroides:
	MOV	R1, TAMANHO_PILHA	; tamanho em palavras da pilha de cada processo
	MUL	R1, R11				; TAMANHO_PILHA vezes o nº da instância da sonda
	SUB	SP, R1		     	; inicializa SP do asteroide atual
	
	reset_asteroide:
		
		; Reseta o bit de deteçao de colisao
		MOV R0, R11							; copia do indice
		SHL R0, 1							; offset para o endereço da sonda na tabela de colisoes
		MOV R1, colisao_asteroide
		MOV R2, 0
		MOV [R1+R0], R2
		
		; Escolher se é minerável ou não (e guardar num registo seguro)
		MOV R9, animacao_asteroides_nao_mineravel ; inicia a não minerável
		MOV R0, 4							; valor entre 0 e 3
		CALL gera_valor_aleatorio			; retorna R1 aleatório
		CMP R1, 0	
		JNZ	escolhe_direcao_posicao			; Se for 0, é minerável, qualquer outra opcção mantém-se não mineravel
		MOV R9, animacao_asteroides_mineravel	  ; desta forma, há 25% de chance de ser minerável
		
		escolhe_direcao_posicao:
			; Gerar valor aleatorio
			gerar_posicao:
				MOV R0, 5						; valor entre 0 e 4
				CALL gera_valor_aleatorio		; retorna R1 aleatório
				CALL verifica_direcao           ; verificar se a direcao já nao foi escolhida
				CMP R7,0                        ; se foi, repete a geração de valor aleatorio
				JZ gerar_posicao
			
			;BLOQUEAR A DIRECAO
			MOV R0,direcoes_disponiveis     ; escolhida a direção, temos de a bloquear em memoria 
			MOV R3,R1						; as direções estao numeradas de 0 a 4,mas declaradas numa tabela de words, logo para aceder
			SHL R3,1                        ; temos de multiplicar o indice da nossa direção por 2
			MOV R2,0
			MOV [R0+R3],R2                  ; bloquear a direção
			MOV R6,R3                       ; endereço da posição que foi disable (para posteriormente ser enable) - NAO PODE SER ALTERADO   
			MOV R8, R1						; guardar o indice aleatorio para a posição do asteroide (coluna)
			
			; Calcular a linha e a coluna inicial do asteroide
			MOV R1, LINHA_ASTEROIDE			; linha em que o asteroide começa
			SHL R8, 2						; offset para o endereço da coluna
			MOV R0, direcoes_colunas_aleatorias
			ADD R0, R8						; endereço da coluna aleatoria
			MOV R2, [R0]					; coluna em que o asteroide começa
			
			; Atualizar a linha e a coluna do asteroide em memoria
			MOV R0, posicao_asteroides_atual ; endereço das posicoes dos asteroides
			MOV R10, R11					; copia do indice do asteroide
			SHL R10, 2						; offset para o endereço da linha do asteroide
			ADD R0, R10						; endereço da linha do asteroide
			MOV [R0], R1					; atualiza a linha do asteroide em memoria
			MOV [R0+2], R2					; atualiza a coluna do asteroide em memoria
			
			; Calcular o movimento horizontal do asteroide
			MOV R0, direcoes_colunas_aleatorias
			ADD R0, R8							; endereço da coluna aleatoria
			ADD R0, 2							; endereço da movimentaçao horizontal aleatoria
			MOV R8, [R0]						; valor da movimentação horizontal aleatória
			
	MOV R10, CAMADA_BASE_ASTEROIDES
	ADD R10,R11 
	
	; REGISTOS FIXOS DURANTO O CICLO:
	; R10 -> Camada do asteroide 
	; R11 -> Indice
	; R9  -> endereço para a tabela que define o asteroide (mineravel ou nao) (pointer para um pointer)
	; R8  -> Direção horizontal
	; R6  -> indice da direção associada ao asteroide 
	ciclo_asteroide:
		
		; Verificar se o jogo está pausado
		MOV R1, [estado_jogo]
		CMP R1, PARADO
		JZ bloqueio_asteroide
		
		; Desenha o asteroide
		MOV R3, R11		         			; Indice do objeto para argumento
		MOV R4, posicao_asteroides_atual	; Tabela das posicoes dos asteroides
		CALL obtem_posicao_objeto		    ; Obtem a linha e a coluna do asteroide
		MOV R4, [R9]						; Os registos de retorno do obtem_posicao_objeto correspondem
		CALL desenha_objeto        	        ; Desenha o asteroide
		
		; Verificar se o jogo está pausado
		MOV R1, [estado_jogo]
		CMP R1, PAUSADO
		JZ bloqueio_asteroide
		
		MOV R0, eventos_interrupcoes
		MOV R1, [R0] 						; esperar pelo lock correspondente ao movimento
											; dos asteroides (o valor de R1 é irrelevante)
		
		; Apagar o asteroide
		MOV R3, R11		         			; Indice do objeto para argumento
		MOV R4, posicao_asteroides_atual	; Tabela das posicoes dos asteroides
		CALL obtem_posicao_objeto		    ; Obtem a linha e a coluna do asteroide
		MOV R4, [R9]						; Os registos de retorno do obtem_posicao_objeto correspondem
											; aos registos de argumento do desenha_objeto
		CALL apaga_objeto      		        ; Desenha o asteroide
		
		; Testa limites
		MOV R3, R11		         			; Indice do objeto para argumento
		MOV R4, posicao_asteroides_atual	; Tabela das posicoes das asteroides
		CALL obtem_posicao_objeto		    ; Obtem a linha e a coluna do asteroide
		MOV R3, LARGURA_ASTEROIDE
		MOV R4, ALTURA_ASTEROIDE
		CALL testa_limites					; Guarda o valor de retorno em R7, para testar posteriormente
		CMP R7, 0                          ; Já chegou ao limite
		JZ enable                     	   ; Vai dar enable da direção
		
		; Move o asteroide
		MOV R1, 1							; Movimento na vertical
		MOV R0, R8							
		MOV R3, R11							; Indice da asteroide	
		MOV R4, posicao_asteroides_atual	; Endereço da posicao das asteroides		
		CALL move_objeto
		
		; Testa colisão com sonda
		MOV R0, R11						    ; copia de indice
		SHL R0, 1							; offset do indice na tabela de colisoes
		MOV R2, colisao_asteroide
		MOV R1, [R2+R0]						; bit para testar colisao
		CMP R1, 1
		JZ destroi_asteroide				; se houve colisao, sair
		
		JMP ciclo_asteroide
	
	destroi_asteroide:
		
		; Aumentar o score da sessão atual
		MOV R1, [score_atual]
		ADD R1, 1
		MOV [score_atual], R1
	
		; Verifica se é minerável
		MOV R5, animacao_asteroides_mineravel		; A unica instância guardada em registo para comparacao
		CMP R9, R5									; entre minerável e não minerável é o endereço das animações dos asteroides
		JNZ toca_som_nao_mineravel				
		; Se for minerável, toca som relativo ao minerável e adiciona energia
		MOV R2, ENERGIA_ASTEROIDE_MINERAVEL		    ; Adiciona energia relativa 
		CALL altera_energia
		MOV R0, SOM_GOOD_JOB						; Toca o som de good job
		MOV [TOCA_SOM], R0
		
		JMP reset_ciclo_destroi_asteroide
		
		toca_som_nao_mineravel:
		; Se for não minerável, toca som relativo ao não minerável
		MOV R0, SOM_METEORO							; Toca o som de destruir o asteroide
		MOV [TOCA_SOM], R0
		
		reset_ciclo_destroi_asteroide:
			ADD R9, 2								; Endereço que aponta para a primeira animacao
			MOV R5, NUMERO_ANIMACOES_ASTEROIDE		; Contador
			
			ciclo_destroi_asteroide:
				; Desenha a animacao
				MOV R3, R11		         			; Indice do objeto para argumento
				MOV R4, posicao_asteroides_atual	; Tabela das posicoes dos asteroides
				CALL obtem_posicao_objeto		    ; Obtem a linha e a coluna do asteroide
				MOV R4, [R9]						; Os registos de retorno do obtem_posicao_objeto correspondem
													; aos registos de argumento do desenha_objeto
				CALL desenha_objeto        	        ; Desenha a animacao
				
				; Verificar se o jogo está pausado
				jogo_pausado_ciclo_destroi_asteroide:
					YIELD
					MOV R1, [estado_jogo]
					CMP R1, PAUSADO
					JZ jogo_pausado_ciclo_destroi_asteroide
				
				MOV R0, eventos_interrupcoes
				MOV R1, [R0] 						; esperar pelo lock correspondente ao movimento
													; dos asteroides (o valor de R1 é irrelevante)
													
				; Apagar a animacao
				MOV R3, R11		         			; Indice do objeto para argumento
				MOV R4, posicao_asteroides_atual	; Tabela das posicoes dos asteroides
				CALL obtem_posicao_objeto		    ; Obtem a linha e a coluna do asteroide
				MOV R4, [R9]						; Os registos de retorno do obtem_posicao_objeto correspondem
													; aos registos de argumento do desenha_objeto
				CALL apaga_objeto      		        ; Apaga a animação
				
				ADD R9, 2							; Endereço que aponta para a próxima animação
				SUB R5, 1							; Aumenta contador
				JNZ ciclo_destroi_asteroide			; Proxima animacao
	
	; Enable da direção que o asteroide percorreu
	enable:
		MOV R0,direcoes_disponiveis
		MOV R1,1
		MOV [R0+R6],R1
		JMP reset_asteroide
		
		
	bloqueio_asteroide:
		MOV R1, [toggle_pausa_jogo]			; bloqueia aqui até dar toggle
		
		CMP R1, PAUSADO						; Se o ultimo estado for pausado, recomeçar no ciclo
		JZ ciclo_asteroide
		
		JMP reset_asteroide			; Se o ultimo estado for parado, resetar para a posicao de inicio				

; **********************************************************************
; VERIFICA_DIRECAO - Avaliar se a direção escolhida aleatoriamente está disponivel para ser percorrida
;	Argumentos: R1 - indice da coluna na tabela de posições disponiveis 
;   Retorna:    R7 - bit de verificação ( 0 - nao permite, 1 - permite)
; **********************************************************************
verifica_direcao:
	PUSH R1
	PUSH R3
	PUSH R4
	
	MOV R3, direcoes_disponiveis
	SHL R1,1								; Offset para o endereço relativo ao indice de argumento
	
	MOV R4,[R3+R1]
	CMP R4,0        						; Ver se a direção está disponivel
	JZ nao_permite
	
	permite:
		MOV R7,1
		JMP fim_verifica_direcao
	nao_permite:
		MOV R7,0
	
	fim_verifica_direcao:
	POP R4
	POP R3
	POP R1
	RET
	
; **********************************************************************
;
; Processo:
;
; PROCESSO_DECRESCIMO_ENERGIA - Processo responsavel pelo decrescimo periodico de energia
;	
; **********************************************************************
PROCESS SP_inicial_energia

processo_descresce_energia:
	
	MOV R0, eventos_interrupcoes			; esperar pelo timing
	MOV R1, [R0+4]
	
	; Verificar se o jogo está ativo
	MOV R1, [estado_jogo]
	CMP R1, ATIVO
	JNZ bloqueio_energia
	
	MOV R2, DECREMENTO_ENERGIA_PERIODICO	; altera o valor da energia
	CALL altera_energia
	
	JMP processo_descresce_energia
	
	bloqueio_energia:
		MOV R1, [toggle_pausa_jogo]			; bloqueia aqui até dar toggle
	
	JMP processo_descresce_energia

; **********************************************************************
; RESETA_JOGO
; **********************************************************************
reseta_jogo:
	PUSH R0
	PUSH R1
	PUSH R2
	PUSH R4
	
	; Toca o som da nave a iniciar
	MOV R0, SOM_NAVE_INICIAR
	MOV [TOCA_SOM], R0
	
	MOV R0, HIGH_SCORE_INICIO
	MOV [score_atual], R0
	
	MOV R0, [razao_parado]				
	MOV [APAGA_CENARIO_FRONTAL], R0
	
	;ENABLE das direções dos asteroides para controlar as direções repetidas
	MOV R4,direcoes_disponiveis
	MOV R1,10   ; (8+2) de forma a possibilitar o ciclo
	MOV R2,1
	
	reenable_direcoes:    ;enable em todas as direções possiveis
		SUB R1,2
		MOV [R4+R1],R2
		CMP R1,0
		JNZ reenable_direcoes
	
	EI										  ; Permite interrupcoes (geral)  
	
	MOV  [APAGA_ECRÃ], R1	                  ; apaga todos os pixeis já desenhados (o valor de R1 não é relevante)
	MOV	 R1, ECRA_JOGO	                      ; cenário de tela de jogo
    MOV  [SELECIONA_CENARIO_FUNDO], R1        ; seleciona o cenário de fundoo
	
	; Resetar o valor da energia
	MOV R0, valor_energia                     ; Inicializa endereços e valores iniciais
	MOV R1, VALOR_ENERGIA_INICIO     
	MOV [R0], R1					          ; Altera o valor na memória
	CALL display_energia			          ; Atualiza o valor do display
	
	; Desenha a nave
	MOV  R1, LINHA_NAVE				          ; Inicializa argumentos 
	MOV  R2, COLUNA_NAVE		
	MOV	 R4, DEF_NAVE		    
	MOV R10, 0
	CALL desenha_objeto                      ; Desenha a nave 
	
	POP R4
	POP R2
	POP R1
	POP R0
	RET
	
; **********************************************************************
; VERIFICA_COLISAO_ASTEROIDE_SONDA - Verifica se um asteroide colidiu com uma sonda e altera os bits correspondentes na memória
; Argumentos:   R1 - Indice do asteroide
;				R2 - Indice da sonda
; **********************************************************************
verifica_colisao_asteroide_sonda:
	PUSH R1 ; indice do asteroide
	PUSH R2 ; indice da sonda
	PUSH R4 ; tabela de posicoes dos asteroides / tabela de bits de colisao de asteroides
	PUSH R5	; tabela de posicoes das sondas / tabela de bits de colisao de sondas
	PUSH R6 ; coluna/linha do asteroide
	PUSH R7 ; coluna/linha da sonda
	
	MOV R4, posicao_asteroides_atual
	MOV R5, posicao_sondas_atual
	
	SHL R1, 2								; offset para o endereço da linha do asteroide
	SHL R2, 2								; offset para o endereço da linha da sonda
	MOV R6, [R4+R1]							; linha do asteroide
	MOV R7, [R5+R2]							; linha da sonda
	
	CMP R7, R6								; a linha da sonda tem que ser maior que a linha do asteroide
	JLT fim_verifica_colisao_asteroide_sonda	  
	
	ADD R6, ALTURA_ASTEROIDE-1				; a linha da sonda tem que ser menor que o limite inferior do asteroide
	CMP R7, R6
	JGT fim_verifica_colisao_asteroide_sonda
	
	ADD R1, 2								; offset para o endereço da coluna do asteroide
	ADD R2, 2								; offset para o endereço da coluna da sonda
	MOV R6, [R4+R1]							; coluna do asteroide
	MOV R7, [R5+R2]							; coluna da sonda
	SUB R1, 2								; offset para o endereço da linha do asteroide| nota: estas subtrações são feitas para posteriormente
	SUB R2, 2								; offset para o endereço da linha da sonda    |		  bastar um SHR para voltar a ter o indice com o offset inicial
	
	CMP R7, R6								; a coluna da sonda tem que ser maior que a coluna do asteroide
	JLT fim_verifica_colisao_asteroide_sonda	  
	
	ADD R6, LARGURA_ASTEROIDE-1				; a coluna da sonda tem que ser menor que o limite direito do asteroide
	CMP R7, R6
	JGT fim_verifica_colisao_asteroide_sonda
	
	; Verifica todas as condições: houve Colisão
	
	SHR R1, 1								; offset para o indice do asteroide na tabela de colisoes (vai de 2 em 2 porque são words)
	SHR R2, 1								; offset para o indice da sonda na tabela de colisoes (vai de 2 em 2 porque são words)
	
	MOV R4, colisao_asteroide
	MOV R5, colisao_sonda
	MOV R6, 1
	MOV [R4+R1], R6							; Comunica com os processos a existencia de colisao
	MOV [R5+R2], R6							; Comunica com os processos a existencia de colisao
	
	fim_verifica_colisao_asteroide_sonda:
	POP R7
	POP R6
	POP R5
	POP R4
	POP R2
	POP R1
	RET
	
	
; **********************************************************************
; VERIFICA_COLISAO_ASTEROIDE_NAVE - Verifica se um asteroide colidiu com a nave
; Argumentos:   R1 - Indice do asteroide
; **********************************************************************
verifica_colisao_asteroide_nave:
	PUSH R1 ; indice do asteroide
	PUSH R2 ; tabela de posicoes dos asteroides
	PUSH R3 ; coluna/linha do asteroide
	PUSH R4 ; coluna/linha da nave
	PUSH R5 ; guarda a largura da nave
	
	MOV R2, posicao_asteroides_atual
	
	SHL R1, 2								; offset para o endereço da linha do asteroide
	MOV R3, [R2+R1]							; linha do asteroide
	MOV R4, LINHA_NAVE						; linha da nave
	
	ADD R3, ALTURA_ASTEROIDE-1				; a linha da nave tem que ser maior que o limite inferior do asteroide
	CMP R4, R3
	JGT fim_verifica_colisao_asteroide_nave  
	
	ADD R1, 2								; offset para o endereço da coluna do asteroide
	MOV R3, [R2+R1]							; coluna do asteroide
	MOV R4, COLUNA_NAVE+OFFSET_COLUNA_NAVE_COLISAO	; coluna da nave com offset para diminuir a hitbox
	ADD R3, LARGURA_ASTEROIDE-1				; limite direito do asteroide
	
	CMP R3, R4								; o limite direito do asteroide tem que estar à esquerda da nave 
											
	JLT fim_verifica_colisao_asteroide_nave ; o asteroide está à esquerda da nave
	
	
	MOV R3, [R2+R1]							; coluna do asteroide
	MOV R4, COLUNA_NAVE + LARGURA_NAVE-(2*OFFSET_COLUNA_NAVE_COLISAO)	; limite direito da nave com offset para diminuir a hitbox
	CMP R3, R4								; a coluna do asteroide tem que estar à direita do limite direito da nave 
	JGT fim_verifica_colisao_asteroide_nave ; o asteroide está à direita da nave
	
	; Verifica todas as condições: houve Colisão
	
	MOV R1, PARADO
	MOV [estado_jogo], R1
	MOV R1, NAVE_EXPLODIU
	MOV [razao_parado], R1
	
	fim_verifica_colisao_asteroide_nave:
	POP R5
	POP R4
	POP R3
	POP R2
	POP R1
	RET

; **********************************************************************
; GERA_VALOR_ALEATORIO - Gera um valor aleatório
; Argumentos:   R0 - Número de hipóteses
;
; Retorna:	R1 - Valor aleatório entre 0 e (R0-1)
; **********************************************************************
gera_valor_aleatorio:
	PUSH R0
	PUSH R2
	PUSH R3

	MOV  R2, TEC_LIN                    ; endereço do periférico das linhas
	MOV  R3, TEC_COL                    ; endereço do periférico das colunas
	MOV  R1, LINHATESTE					; qualquer linha do teclado (R1 usado como registo temporario)
	MOVB [R2], R1                       ; escrever no periférico de saída (linhas)
	MOVB R1, [R3]                       ; ler do periférico de entrada (colunas)
	SHR  R1, 4							; colocar os bits aleatorios nos 4 menos significativos
	MOD  R1, R0							; cap do valor aleatorio
	
	POP R3
	POP R2
	POP R0
	RET

; **********************************************************************
; PINTA_OBJETO - pinta qualquer objeto no ecrã
; Argumentos:   R1 - linha
;               R2 - coluna
;               R4 - tabela que define o objeto
;				R9 - bit que indica se deve apagar (0) ou não (1)
;               R10 - ECRÃ
; **********************************************************************
pinta_objeto:	
	PUSH R1                                ; linha
	PUSH R2                                ; coluna
	PUSH R3                                ; cor do pixel
	PUSH R4                                ; tabela do objeto
	PUSH R5                                ; largura
	PUSH R6                                ; altura
	PUSH R7                                ; valor_coluna_inicial
	PUSH R8                                ; valor_largura_inicial
	PUSH R9
	PUSH R11
	
	; Verificar se o jogo está PARADO (para evitar bugs quando o jogo acaba)
	MOV R11, [estado_jogo]
	CMP R11, PARADO
	JZ fim_pinta_objeto
	
	MOV [SELECIONA_ECRA],R10
	
	MOV R7, R2                             ; guarda a primeira coluna
	
	MOV	R5, [R4]			               ; obtém a largura do objeto
	MOV R8, [R4]                           ; guarda a largura (para o loop)
	
	ADD	R4, 2			                   ; endereço da altura
	MOV	R6, [R4]			               ; obtém a altura do objeto
	ADD	R4, 2				               ; endereço da cor do 1º pixel
	
	desenha_pixeis_coluna:
	
		desenha_pixeis_linha:       	   ; desenha os pixels do objeto a partir da tabela
			MOV R3,0              
			CMP R9,0                       ; bit que indica se deve apagar(0) ou não (1)
			JZ escreve_pixels              ; se for para apagar tem de saltar a proxima instrução em todos os ciclos
			MOV	 R3, [R4]			       ; obtém a cor do próximo pixel do objeto
		escreve_pixels:
			CALL escreve_pixel		       ; escreve cada pixel do objeto
			ADD	 R4, 2		               ; endereço da cor do próximo pixel (2 porque cada cor de pixel é uma word)
			ADD  R2, 1                     ; próxima coluna
			SUB  R5, 1			           ; menos uma coluna para tratar
			JNZ  desenha_pixeis_linha      ; continua até percorrer toda a largura do objeto
	
		MOV R2, R7                         ; reseta as colunas
		MOV R5, R8                         ; reseta a largura
		ADD R1, 1                          ; passa para a proxima linha
		SUB R6, 1                          ; menos uma linha para tratar 
		JNZ desenha_pixeis_coluna
	
	MOV [MOSTRA_ECRA],R10
	
	fim_pinta_objeto:
	POP R11
	POP R9
	POP R8
	POP R7
	POP R6
	POP	R5
	POP	R4
	POP	R3
	POP	R2
	POP R1
	RET

; **********************************************************************
; APAGA_OBJETO - Apaga um objeto.
; Argumentos:   R1 - linha
;               R2 - coluna
;               R4 - tabela que define o objeto
;				R10 - ecra
; **********************************************************************
apaga_objeto:	
	PUSH R9
	PUSH R10
	
	MOV R9,0
	CALL pinta_objeto
	
	POP R10
	POP R9
	RET
	
; **********************************************************************
; DESENHA_OBJETO - desenha um objeto.
; Argumentos:   R1 - linha
;               R2 - coluna
;               R4 - tabela que define o objeto
;				R10 - ecra
; **********************************************************************
desenha_objeto:	
	PUSH R9
	PUSH R10

	MOV R9,1
	CALL pinta_objeto

	POP R10
	POP R9
	RET

; **********************************************************************
; ESCREVE_PIXEL - Escreve um pixel na linha e coluna indicadas.
; Argumentos:   R1 - linha
;               R2 - coluna
;               R3 - cor do pixel (em formato ARGB de 16 bits)
; **********************************************************************
escreve_pixel:
	MOV  [DEFINE_LINHA], R1		         ; seleciona a linha
	MOV  [DEFINE_COLUNA], R2	         ; seleciona a coluna
	MOV  [DEFINE_PIXEL], R3		         ; altera a cor do pixel na linha e coluna já selecionadas
	RET

; **********************************************************************
; TESTA_LIMITES - Testa se o objeto chegou aos limites do ecrã 
; Argumentos:	R1 - linha em que o objeto está
;				R2 - coluna em que o objeto está
;				R3 - largura do objeto
;				R4 - altura do objeto
; Retorna: 	R7 - 0 se já tiver chegado ao limite, 1 caso contrário	
; **********************************************************************
testa_limites:
	PUSH 	R1
	PUSH 	R2
	PUSH 	R3
	PUSH	R4
	PUSH	R5
	PUSH 	R6
	
	MOV R7, 1					 		 ; admite-se que o objeto pode continuar
	
	; Nota: Não é preciso testar o limite superior porque a rotina é apenas utilizada pelos asteroides
	
	; Adaptação do testa limite esquerdo para 
	; fazer com que o movimento do asteroide 
	; funcione como suposto
	testa_limite_esquerdo:		         ; vê se o objeto chegou ao limite esquerdo (em função do pivot do canto superior direito)
		MOV	R5, MIN_COLUNA
		MOV R6, R2						 ; guarda o valor da coluna
		ADD R6, R3						 ; passa a coluna para o pivot desejado
		SUB R6, 1						 ; fix do valor da nova coluna
		CMP	R6, R5			         ; se o extremo direito for igual ao minimo, deve travar
		JGT	testa_limite_direito         ; caso contrário, testar mais limites
		JMP	impede_movimento	
		
	testa_limite_direito:		         ; vê se o objeto chegou ao limite direito
		ADD	R3, R2			             ; posição do extremo direito do objeto
		MOV	R5, MAX_COLUNA
		CMP	R3, R5				         ; se o extremo direito for igual ao máximo, deve travar
		JLE	testa_limite_baixo	         ; caso contrário, testar mais limites
		JMP	impede_movimento
	
	testa_limite_baixo:			          
		MOV	R5, MAX_LINHA
		CMP	R1, R5				         ; se o extremo inferior do objeto for maior que o máximo, deve travar
		JLE	sai_testa_limites	         ; caso contrário, passou em todos os limites
		JMP	impede_movimento
		
	impede_movimento:
		MOV	R7, 0			             ; impede o movimento, forçando R7 a 0
	sai_testa_limites:
		POP R6
		POP	R5
		POP	R4
		POP R3
		POP R2
		POP R1
		RET
		
; **********************************************************************
; MOVE_OBJETO - Move um objeto
; Argumentos:  R0 - movimento lateral
;			   R1 - movimento vertical
;			   R3 - indice do objeto na tabela de posicoes
;			   R4 - tabela de posicoes
; **********************************************************************
move_objeto:
	PUSH R0
	PUSH R1
	PUSH R3
	PUSH R4
	PUSH R5
	PUSH R6
	
	SHL R3, 2							; Cada objeto tem movimento vertical e lateral (ocupa 4 bytes) assim,
										; obtemos o offset para o endereço do movimento vertical da sonda atual
	
	ADD R4, R3							; Endereço da linha atual
	
	MOV R6, [R4]						; Guarda o valor da linha atual
	MOV R5, [R4+2]						; Guarda o valor da coluna atual
	
	ADD R5, R0							; Adiciona o valor a mover lateralmente
	ADD R6, R1							; Adiciona o valor a mover verticalmente
	
	MOV [R4], R6						; Guarda o novo valor da linha 
	MOV [R4+2], R5						; Guarda o novo valor da coluna 
	
	POP R6
	POP R5
	POP R4
	POP R3
	POP R1
	POP R0
	RET

; **********************************************************************
; RESETA_OBJETO - Reseta um objeto
; Argumentos:  R2 - indicador do objeto ("indice" que ocupa na tabela de posicoes)
;			   R3 - tabela de posicoes atuais
;			   R4 - tabela de posicoes iniciais
; **********************************************************************
reseta_objeto:
	PUSH R0 ; valor inicial da linha
	PUSH R1 ; valor inicial da coluna
	PUSH R2
	PUSH R3
	PUSH R4
	
	SHL R2, 2                           ; Cada objeto tem linha e coluna (ocupa 4 bytes) assim,
										; obtemos o offset para o endereço da linha do objeto que queremos mover
		
	ADD R4, R2							; Endereço da linha inicial  
	ADD R3, R2                          ; Endereço da linha atual
	
	MOV R0, [R4]						; Encontra o valor inicial da linha
	MOV R1, [R4+2]						; Encontra o valor inicial da coluna
	
	MOV [R3], R0						; Guarda o novo valor da linha 
	MOV [R3+2], R1						; Guarda o novo valor da coluna 
	
	POP R4
	POP R3
	POP R2
	POP R1
	POP R0
	RET

; **********************************************************************
; OBTEM_POSICAO_OBJETO - Obtém a posicão de um objeto
; Argumentos:  R3 - indicador do objeto ("indice" que ocupa na tabela de posicoes)
;			   R4 - tabela de posicoes
;	
; Retorna: R1 - linha do objeto
;	       R2 - coluna do objeto
; **********************************************************************
obtem_posicao_objeto:
	PUSH R3
	PUSH R4
	
	SHL R3, 2                           ; Cada objeto tem linha e coluna (ocupa 4 bytes) assim,
										; obtemos o offset da linha do objeto que queremos mover
		
	ADD R4, R3                          ; Endereço da linha do objeto a mover
	MOV R1, [R4]						; Guarda o valor da linha atual
	MOV R2, [R4+2]						; Guarda o valor da coluna atual
	
	POP R4
	POP R3
	RET

; **********************************************************************
; ITERA_LINHAS - Verifica qual é a próxima linha a testar
; Argumentos:	R1 - ultima linha lida
;
; Retorna: 	R1 - proxima linha	
; **********************************************************************
itera_linhas:
	JMP proxima_linha
	reset_linha:
		MOV  R1, LINHAOFFSET
	proxima_linha:                      ; passa para a proxima linha    
		SHR R1, 1         
		CMP R1, 0	
		JZ reset_linha
	RET

; **********************************************************************
; CALCULA_VALOR_TECLADO - Calcula a tecla exata em que o utilizador carregou
; Argumentos:	R1 - linha
;				R0 - coluna
;
; Retorna: 	R2 - valor lido no teclado	
; **********************************************************************
calcula_valor_teclado:
	PUSH R3
	MOV  R2, 0                          ; inicializar o valor lido no teclado a 0
	
	MOV  R3, R1                         ; o valor temporario começa no valor da linha
	CALL contador_linha_coluna

	SUB R2, 1                           ; fix ao valor lido
	SHL R2, 2                           ; multiplica por 4

	MOV  R3, R0                         ; o valor temporario começa no valor da coluna
	CALL contador_linha_coluna

	SUB R2, 1                           ; fix ao valor lido
	
	POP R3
	RET
	
; **********************************************************************
; CONTADOR_LINHA_COLUNA - converte uma linha para um valor de 0 a 3
; Argumentos:	R3 - linha/coluna
;               R2 - valor acumulado no calculo
;
; Retorna: 	R2 - contador
; **********************************************************************
contador_linha_coluna:
	ADD R2, 1                           ; adiciona ao valor lido
	SHR R3, 1                           ; altera a linha a contar
	JNZ contador_linha_coluna           ; se nao acabar, conta mais linhas
	RET
	
; **********************************************************************
; ALTERA_ENERGIA - altera o valor da energia
; Argumentos:   R2 - valor a alterar
; **********************************************************************
altera_energia:
	PUSH R0
	PUSH R1
	PUSH R2
	PUSH R3
	MOV R0, valor_energia               ; inicializa o endereço do valor de energia
	MOV R1, [R0]                        ; valor atual da energia, para ser alterado
	ADD R1, R2                          ; adiciona o valor guardado no registo
	CMP R1, 0  
	
	JGT alteracao_energia_memoria 		; dar cap no valor da energia para nunca
										; ser inferior a 0 e comunicar que o jogador perdeu
	MOV R1, 0
	MOV R3, PARADO
	MOV [estado_jogo], R3				; o jogador perdeu
	MOV R3, ACABOU_ENERGIA
	MOV [razao_parado], R3				; atualiza a razão do jogador ter perdido
	
	alteracao_energia_memoria:
	MOV [R0], R1                        ; atualiza o valor da energia na memória
	CALL display_energia		        ; Mostra o valor da energia
	POP R3
	POP R2
	POP R1
	POP R0
	RET

; **********************************************************************
; DISPLAY_ENERGIA - dá display ao valor de energia guardado na memória
; **********************************************************************
display_energia:
	PUSH R0						        ; endereço da energia
	PUSH R1						        ; valor temporario de energia (vai sendo alterado)
	PUSH R2						        ; valor das unidades
	PUSH R3						        ; valor das dezenas
	PUSH R4						        ; valor das centenas
	PUSH R5                             ; valor temporário para calculos
	PUSH R6                             ; valor final
	
	MOV R6, 0H                          ; valor inicial
	
	MOV R0, valor_energia               ; endereço valor energia
	MOV R1, [R0]                        ; valor da energia
	
	MOV R5, 10                          ; para obter o ultimo digito do valor atual
	
	; Cálculo das unidades
	MOV R2, R1                          ; cópia do valor original para não o perder
	MOD R2, R5                          ; obter o digito das unidades
	
	; Cálculo das dezenas
	SUB R1, R2 					        ; ficar com o digito das unidades a 0
	DIV R1, R5                          ; "eliminar" o digito das unidades
	MOV R3, R1 					        ; cópia do valor original para não o perder
	MOD R3, R5                          ; obter o digito das unidades (correspondente às dezenas do valor original) 
	
	; Cálculo das centenas
	SUB R1, R3 					        ; ficar com o digito das unidades a 0
	DIV R1, R5                          ; "eliminar" o digito das unidades
	MOV R4, R1 					        ; cópia do valor original para não o perder
	MOD R4, R5                          ; obter o digito das unidades (correspondente às centenas do valor original)
	
	; Fix aos valores para "enviar" para o display em "decimal"
	SHL R4, 8					        ; as centenas devem ocupar os bits de 11-8
	SHL R3, 4                           ; as dezenas devem ocupar os bits de 7-4
	OR  R6, R2                          ; "adiciona" as unidades
	OR  R6, R3                          ; "adiciona" as dezenas
	OR  R6, R4                          ; "adiciona" as centenas
	
	MOV R0, DISPLAYS                    ; endereço do display
	MOV [R0], R6                        ; atualiza o display
	
	POP R6
	POP R5
	POP R4
	POP R3
	POP R2
	POP R1
	POP R0
	RET

; **********************************************************************
; Rotinas de interrupção 
; **********************************************************************

; **********************************************************************
; MOVE_ASTEROIDES -	Rotina de movimento dos asteroides
;			Desbloqueia o movimento	dos asteroides
; **********************************************************************
move_asteroides:
	PUSH R0
	MOV R0, eventos_interrupcoes
	MOV [R0],R1 					; alterar o lock correspondente ao movimento
									; dos asteroides (o valor de R1 é irrelevante)
	POP R0
	RFE
	
; **********************************************************************
; MOVE_SONDAS -	Rotina de movimento das sondas
;			Desbloqueia o movimento das sondas
; **********************************************************************
move_sondas:
	PUSH R0
	MOV R0, eventos_interrupcoes
	MOV [R0+2],R1 					; alterar o lock correspondente ao movimento
									; das sondas (o valor de R1 é irrelevante)
	POP R0
	RFE

; **********************************************************************
; DECRESCE_ENERGIA_PERIODICAMENTE -	Rotina de atendimento de alteracao da energia
;			Vai descrescendo o valor de energia ao longo do tempo
; **********************************************************************
decresce_energia_periodicamente:
	PUSH R0
	MOV R0, eventos_interrupcoes
	MOV [R0+4],R1 					; alterar o lock correspondente à alteração do
									; valor da energia (o valor de R1 é irrelevante)
	POP R0
	RFE
	
; **********************************************************************
; DESBLOQUEIA_PADRAO_NAVE -	Rotina de alteracao do padrao da nave
;			Altera o padrão da nave
; **********************************************************************
desbloqueia_altera_padrao_nave:
	PUSH R0 
	MOV R0, eventos_interrupcoes
	MOV [R0+6],R1 					; alterar o lock correspondente ao padrão
									; da nave (o valor de R1 é irrelevante)
	POP R0
	RFE