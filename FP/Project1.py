# 1. Justificação de textos

def limpa_texto (texto):
    '''
    Esta função recebe uma cadeia de carateres qualquer e devolve a cadeia de carateres limpa, ou seja, sem espaços iniciais, finais ou consecutivos e sem nenhum carater de escape (\n,\t,\v,\r,\f).

    limpa_texto: {cad.caracteres} --> {cad.caracteres}
    '''
    # À lista de palavras do texto é adicionado um espaço entre as palavras.
    return " ".join(texto.split()) 
   
def corta_texto (texto, largura_coluna):
    '''
    Esta função recebe uma cadeia de carateres e um inteiro positivo correspondentes um texto limpo e uma largura de coluna respetivamente, 
    e devolve duas subcadeias de carateres limpas: a primeira contendo todas as palavras completas desde o início da cadeia original 
    (incluindo os espaços separadores entre elas) até um comprimento máximo total igual à largura fornecida, e a segunda cadeia contendo o resto do texto de entrada. 
    Ambas estas cadeias não começam ou terminam com um espaço e podem ser vazias dependendo do valor dos parâmetros concretos de entrada. 

    corta_texto: {cad.carateres} x {inteiro} -→ {cad.carateres} x {cad.caracteres}
    '''
    if len(texto) <= largura_coluna:  
        return (texto, "") 

    # Verifica quantos espaços, ou seja, quantas palavras completas existem na primeira parte do texto que vai ser cortado (string_1).
    número_de_espaços = 0
    i= 0
    while i <= largura_coluna:
        if texto[i] == " ":
            número_de_espaços += 1
        i += 1

    # Corta o texto em duas partes (string_1 e string_2).  
    lista_de_palavras = texto.split()   
    if número_de_espaços > 0:
        string_1 = " ".join(lista_de_palavras[0:número_de_espaços]) 
        string_2 = " ".join(lista_de_palavras[número_de_espaços: ])  

    else: 
        # Como não existem espaços na string_1, então não existe também nenhuma palavra completa na string_1.
        string_1 = ""  
        string_2 = texto

    return (string_1, string_2) 

def insere_espacos (texto, largura_coluna):
    '''
    Esta função recebe uma cadeia de carateres e um inteiro positivo correspondentes a um texto limpo e uma largura de coluna respetivamente, e no caso da cadeia de entrada
    conter duas ou mais palavras devolve uma cadeia de carateres de comprimento igual à largura da coluna formada pela cadeia original com espaços entre palavras conforme descrito. 
    Caso contrário, devolve a cadeia de comprimento igual à largura da coluna formada pela cadeia original seguida de espaços. 

    insere_espacos: {cad.carateres} x {inteiro} -→ {cad.carateres} 
    '''
    lista_de_palavras = texto.split()  
    número_de_palavras = len (lista_de_palavras)
    espaços_existentes = número_de_palavras - 1   
    espaços_a_adicionar = largura_coluna - len(texto)  

    # Neste caso são adicionados espaços entre as palavras do texto.
    if número_de_palavras > 1: 
        quociente, resto = divmod(espaços_a_adicionar, espaços_existentes)
        for i in range(espaços_existentes):
            lista_de_palavras[i] += " "* quociente + (" " if i < resto else "")
        
        return (" ".join(lista_de_palavras))

    # Neste caso são adicionados espaços à direita da palavra.
    else:    
        return texto.ljust(largura_coluna, " ")

def justifica_texto(texto, largura_coluna):
    '''
    Esta função recebe uma cadeia de carateres não vazia e um inteiro positivo correspondentes a um texto qualquer e uma largura de coluna respetivamente,
    e devolve um tuplo de cadeias de carateres justificadas, isto é, de comprimento igual à largura da coluna com espaços entre palavras conforme descrito. 
    Esta função verifica a validade dos seus argumentos.

    justifica_texto: {cad. carateres} x {inteiro} -→ {tuplo}
    '''
    # O texto tem de ser uma cadeia de caracteres não vazia.
    if type(texto) != str or len(texto) == 0:
        raise ValueError ("justifica_texto: argumentos invalidos")

    # O número de colunas tem de ser um inteiro positivo.
    if type(largura_coluna) != int or largura_coluna <= 0:
        raise ValueError ("justifica_texto: argumentos invalidos")

    # As palavras do texto de entrada devem ter tamanho igual ou menor à largura da coluna pretendida.
    lista_palavras_texto = texto.split()
    número_palavras_texto = len (lista_palavras_texto)
    for i in range(número_palavras_texto):
        tamanho_da_palavra = len (lista_palavras_texto[i])
        if tamanho_da_palavra > largura_coluna:
            raise ValueError ("justifica_texto: argumentos invalidos")
    
    # Neste caso é preciso justificar o texto limpo        
    texto_limpo = limpa_texto(texto)
    texto_final = []
    if len (texto_limpo) != largura_coluna:
        
        # Este loop serve para ir cortanto o texto e inserindo os espaços necessários, de modo a ser possível ir armazenando numa lista o texto final.
        i = 0
        while i < len(lista_palavras_texto):
            texto_cortado = corta_texto(texto_limpo, largura_coluna)

            # Quando esta condição se verificar quer dizer que o texto limpo original já foi todo cortado em partes e por isso não faz sentido continuar o loop.
            if texto_cortado[0] == "":
                break

            texto_limpo = texto_cortado[1]

            # Na última linha, o texto é justificado adicionando espaços à sua direita.
            if texto_limpo == "":
                texto = texto_cortado[0].ljust(largura_coluna, " ")
                texto_final.append(texto)

            # Para todas as outras linhas de texto, os espaços são adicionados entre as palavras do texto.
            else:
                texto = insere_espacos(texto_cortado[0], largura_coluna)
                texto_final.append(texto)
            i += 1
        
        return tuple(texto_final)
        
    # Neste caso o texto limpo já vem justificado porque a cadeira de caracteres tem o mesmo comprimento que a largura de coluna predefinida.    
    else: 
        return (texto_limpo,)
        

# 2. Método de Hondt

def calcula_quocientes (votos_apurados, número_deputados):
    '''
    Esta função recebe um dicionário com os votos apurados num círculo eleitoral (com pelo menos um partido) e um inteiro positivo representando o número de deputados; 
    e devolve o dicionário com as mesmas chaves do dicionário argumento (correspondente a partidos) contendo a lista (de comprimento igual ao número de deputados)
    com os quocientes calculados com o método de Hondt ordenados em ordem decrescente. 
    
    calcula_quocientes: {dicionário} x {inteiro} -→ {dicionário}
    '''
    # Para cada partido vão ser calculados os quocientes dos seus votos.
    quocientes_dos_votos = {} 
    for partido in votos_apurados:    
        lista_de_votos = []  # Esta lista vai ter os quocientes dos votos de cada um dos partidos

        divisor = 1
        while divisor <= número_deputados:
            lista_de_votos.append(votos_apurados[partido] / divisor)
            divisor += 1

        quocientes_dos_votos[partido] = lista_de_votos 

    return quocientes_dos_votos

def atribui_mandatos (votos_apurados, número_deputados):
    ''' 
    Esta função recebe um dicionário com os votos apurados num círculo eleitoral e um inteiro positivo representando o número de deputados, e devolve a lista ordenada de tamanho igual ao número de deputados 
    contendo as cadeias de carateres dos partidos que obtiveram cada mandato, isto é, a primeira posição da lista corresponde ao nome do partido que obteve o primeiro deputado, 
    a segunda ao partido que obteve o segundo deputado, etc. 
    No caso de existirem dois ou mais partidos com igual quociente, os mandatos são distribuídos por ordem ascendente as listas menos votadas.

    atribui_mandatos: {dicionário} x {inteiro} -→ {lista}
    '''
    lista_mandatos = []
    quocientes_votos = calcula_quocientes (votos_apurados, número_deputados)
    while len(lista_mandatos) < número_deputados:
        
        # O maior quociente de votos de cada um dos partidos vai ser apreendido para uma lista
        lista_quocientes = []
        for partido, votos in quocientes_votos.items():
            lista_quocientes.append(votos[0])  # votos[0] é o maior quociente de votos de cada um dos partidos
        lista_quocientes = sorted(lista_quocientes)  
        
        # Verifica quantos partidos têm o maior quociente de votos.
        contador = 0
        for partido, votos in quocientes_votos.items():
            if lista_quocientes[-1] in votos:
                contador += 1
        
        # Se só um partido tem o maior quociente de votos, então a esse partido vai ser atribuido um mandato.
        if contador == 1: 
            for partido,votos in quocientes_votos.items():
                if lista_quocientes[-1] in votos:
                    lista_mandatos.append(partido)
                    del (quocientes_votos[partido][0])  
        
        # Se existem dois ou mais partidos com o maior quociente de votos, então o mandato é atribuido ao partido menos votado.  
        else:
            lista_desempate = []
            for partido,votos in quocientes_votos.items():
                if lista_quocientes[-1] in votos:
                    lista_desempate.append(partido)      

            while len(lista_desempate) > 1:
                if votos_apurados[lista_desempate[0]] < votos_apurados [lista_desempate[1]]:
                    del (lista_desempate[1])

                elif votos_apurados[lista_desempate[0]] > votos_apurados[lista_desempate[1]]:
                    del (lista_desempate[0])
                    
            lista_mandatos.append(lista_desempate[0])
            del (quocientes_votos[lista_desempate[0]][0])

    return lista_mandatos

def obtem_partidos (info):
    '''
    Esta função recebe um dicionário com a informação sobre as eleições num território com vários círculos eleitorais como descrito,
    e devolve a lista por ordem alfabética com o nome de todos os partidos que participaram nas eleições.

    obtem_partidos: {dicionário} -→ {lista}
    '''
    lista_partidos = []
    i = 0   
    while i < len (info):
        for círculo_eleitoral, info_círculo in info.items():
            
            for chave, valor in info_círculo.items():

              # A chave votos é acedida para ser possível obter os partidos.  
              if chave == "votos":
                for partido, votos in valor.items():
                    # A lista só vai apreender cada partido uma única vez.
                    if partido not in lista_partidos:
                        lista_partidos.append(partido)
        i += 1
    
    return sorted (lista_partidos)

def obtem_resultado_eleicoes(info):
    '''
    Esta função recebe um dicionário com a informação sobre as eleições num território com vários círculos eleitorais como descrito, 
    e devolve a lista ordenada de comprimento igual ao número total de partidos com os resultados das eleições. 
    Cada elemento da lista é um tuplo de tamanho 3 contendo o nome de um partido, o número total de deputados obtidos e o número total de votos obtidos.
    A lista está ordenada por ordem descendente de acordo ao número de deputados obtidos e, em caso de empate, de acordo ao número de votos.

    obtem_resultado_eleicoes: {dicionário} -→ {lista}
    '''
    # A informação tem de estar num dicionário não vazio.
    if type (info) != dict or len(info) == 0:
        raise ValueError ("obtem_resultado_eleicoes: argumento invalido")
    
    lista_mandatos = []
    for círculo_eleitoral, info_círculo in info.items():

        # Os círculos eleitorais têm de ser cadeias de caracteres.
        if type(círculo_eleitoral) != str:
            raise ValueError ("obtem_resultado_eleicoes: argumento invalido")

        # Os valores de info têm de ser dicionários com duas chaves ("deputados" e "votos").
        if type(info_círculo) != dict or len(info_círculo) != 2 or "deputados" not in info_círculo or "votos" not in info_círculo:
            raise ValueError ("obtem_resultado_eleicoes: argumento invalido")
        
        # O número de deputados tem de ser um inteiro positivo.
        if type(info_círculo["deputados"]) != int or info_círculo["deputados"] <= 0 :
            raise ValueError ("obtem_resultado_eleicoes: argumento invalido")
        
        # Os votos têm de estar num dicionário não vazio.
        if type(info_círculo["votos"]) != dict or len(info_círculo["votos"]) == 0:
           raise ValueError ("obtem_resultado_eleicoes: argumento invalido")
        
        contador_votos = 0
        for partido, votos in info_círculo["votos"].items():

            # O nome dos partidos tem de ser uma cadeia de caracteres.
            if type(partido) != str:
                raise ValueError ("obtem_resultado_eleicoes: argumento invalido")
            
            # Os votos têm de ser inteiros positivos.
            if type(votos) != int or votos <= 0:
                raise ValueError ("obtem_resultado_eleicoes: argumento invalido")  
            contador_votos += votos 

        # Para cada círculo eleitoral tem de haver pelo menos um partido a receber votos.
        if contador_votos == 0:
            raise ValueError ("obtem_resultado_eleicoes: argumento invalido")

        lista_mandatos.append(atribui_mandatos(info_círculo["votos"], info_círculo["deputados"]))

    lista_partidos = obtem_partidos(info)
    lista_final = []

    # Cálculo do número de deputados atribuidos.
    for partido in lista_partidos:
        número_deputados = 0
        i = 0
        while i < len(lista_mandatos):
            for e in lista_mandatos[i]:
                if (e == partido):
                    número_deputados += 1
            i += 1
        
        # Cálculo do número total de votos.
        votos_totais = 0
        for círculo_eleitoral, info_círculo in info.items():
            for k, v in info_círculo["votos"].items():
                if (k == partido):
                    votos_totais += v

        lista_final.append((partido,número_deputados,votos_totais))
        votos_totais = 0
        
    
    def ordenar_resultados (e):
        '''
        Esta função auxiliar permite ordenar resultados
        '''
        return e[1], e[2]
    
    lista_final.sort(key = ordenar_resultados, reverse = True)
    return lista_final


# 3. Solução de Sistemas de Equações

def produto_interno(vetor_1, vetor_2):
    '''
    Esta função recebe dois tuplos de números (inteiros ou reais) com a mesma dimensão 
    representando dois vetores e retorna o resultado do produto interno desses dois vetores.

    produto_interno: {tuplo} x {tuplo} -→ {real}
    '''
    # Cálculo do produto interno dos 2 vetores.
    produto = 0.0
    res = 0.0
    i = 0
    for elemento in vetor_1:
        produto = vetor_1 [i] * vetor_2 [i]
        res = res + produto
        produto = 0
        i += 1
    return float(res)

def verifica_convergencia(matriz_A, vetor_c, solução_x, precisão_ϵ):
    '''
    Esta função recebe três tuplos de igual dimensão e um valor real positivo. 
    O primeiro tuplo é constituído por um conjunto de tuplos cada um representando uma linha da matriz quadrada A, 
    e os outros dois tuplos de entrada contêm valores representando respetivamente o vetor de constantes c e a solução atual x.
    O valor real de entrada indica a precisão pretendida ϵ. 

    verifica_convergencia: {tuplo} x {tuplo} x {tuplo} x {real} -→ {booleano}
    '''
    # Cálculo dos elementos de f(x)
    fx = []
    i = 0
    for elemento in matriz_A:
        fx.append(produto_interno(matriz_A[i], solução_x))
        i += 1

    # Verificação do valor absoluto do erro de cada uma das equações. 
    # Se em alguma destas o valor absoluto do erro for maior que a precisão ε, então a função irá retornar False. Caso contrário retorna True.
    i = 0
    for elemento in fx:
        if abs(fx[i] - vetor_c[i]) >= precisão_ε:
            return False
        i += 1
    return True

def retira_zeros_diagonal (matriz_entrada, vetor_constantes):
    '''
    Esta função recebe um tuplo de tuplos, representando a matriz de entrada no mesmo formato das funções anteriores, e um tuplo de números, representando o vetor das constantes. 
    A função deverá retornar uma nova matriz com as mesmas linhas que a de entrada, mas com estas reordenadas de forma a não existirem valores 0 na diagonal. 
    O segundo parâmetro de saída é também o vetor de entrada com a mesma reordenação de linhas que a aplicada à matriz.

    retira_zeros_diagonal: {tuplo} x {tuplo} -→ {tuplo} x {tuplo}
    '''
    # Verifica se existem zeros na diagonal da matriz
    matriz_entrada = list(matriz_entrada)
    vetor_constantes = list(vetor_constantes)
    for i in range(len(matriz_entrada)):
        if matriz_entrada[i][i] == 0:
            
            # Procura desde o início da matriz, uma linha j que não contenha um zero na diagonal.
            for j in range(len(matriz_entrada)):
                if matriz_entrada[j][i] != 0 and matriz_entrada[i][j] != 0:
                    
                    # A linha i que tem zero na diagonal é trocada pela linha j que não tem zero na diagonal.
                    matriz_entrada[i], matriz_entrada[j] = matriz_entrada[j], matriz_entrada[i]
                    vetor_constantes[i], vetor_constantes[j] = vetor_constantes[j], vetor_constantes[i]
                    break

    return (tuple (matriz_entrada), tuple (vetor_constantes))
    
def eh_diagonal_dominante (matriz):
    '''
    Esta função recebe um tuplo de tuplos representando uma matriz quadrada no mesmo formato das funções anteriores. 
    Vai retornar True caso seja uma matriz diagonalmente dominante, e False caso contrário.

    eh_diagonal_dominante: {tuplo} -→ {booleano}
    '''
    # Cálculo tanto do valor absoluto da diagonal como do valor absluto da soma dos restantes valores da linha, para cada um dos elementos da matriz.
    i = 0 
    for tuplo in matriz:
        valor_diagonal = abs(tuplo[i])
        valor_linha = 0
        
        j = 0
        while j < len(tuplo):
            if j != i:
                valor_linha += abs(tuplo[j])
            j += 1
        i += 1

        # Se houver algum elemento da matriz onde o valor absoluto da diagonal seja menor do que o valor absoluto 
        # da soma dos restantes valores da linha, então a matriz não é diagonalmente dominante.
        if abs(valor_diagonal) < abs(valor_linha):
            return False
     
    return True

def resolve_sistema (matriz, vetor_constantes, precisão):
    '''
    Esta função recebe um tuplo de tuplos representando uma matriz quadrada no mesmo formato das funções anteriores correspondente aos coeficientes das equações do sistema,
    um tuplo de números representando o vetor das constantes, e um valor real positivo correspondente à precisão pretendida para a solução. 
    Retorna um tuplo que é a solução do sistema de equações de entrada aplicando o método de Jacobi.

    resolve_sistema: {tuplo} x {tuplo} x {real} -→ {tuplo}
    '''
    # A matriz tem de ser um tuplo não vazio
    if type(matriz) != tuple or len(matriz) == 0 :
        raise ValueError ("resolve_sistema: argumentos invalidos")

    número_de_linhas = len(matriz)
    i = 0
    while i < número_de_linhas:

        # A matriz tem de ser quadrada, ou seja o comprimento da matriz tem de ser igual ao comprimento de cada um dos seus elementos.
        if type(matriz[i]) != tuple or len(matriz[i]) != número_de_linhas:
            raise ValueError ("resolve_sistema: argumentos invalidos") 

        # Cada um dos elementos da matriz tem de ser inteiro ou real.
        for elemento in matriz[i]:
            if type(elemento) != int and type(elemento) != float: 
                raise ValueError ("resolve_sistema: argumentos invalidos")
        i += 1
        
    # O vetor das constantes tem de ser um tuplo não vazio com comprimento igual ao número de linhas da matriz.
    if type(vetor_constantes) != tuple or len(vetor_constantes) == 0 or número_de_linhas != len(vetor_constantes):
        raise ValueError ("resolve_sistema: argumentos invalidos")

    # Cada um dos elementos do vetor das constantes tem de ser um inteiro ou um real.
    for elemento in vetor_constantes:
        if type(elemento) != int and type(elemento) != float:
            raise ValueError ("resolve_sistema: argumentos invalidos")

    # A precisão tem de ser um valor real positivo
    if type(precisão) != float  or precisão <= 0:
        raise ValueError ("resolve_sistema: argumentos invalidos")   
     
    matriz, vetor_constantes = retira_zeros_diagonal(matriz, vetor_constantes) 

    if not eh_diagonal_dominante(matriz):
            raise ValueError ("resolve_sistema: matriz nao diagonal dominante")

    # Criação de dois tuplos vazios com comprimento igual ao da matriz.       
    solução_x = ()
    solução_final =()
    i = 0
    while i < len(matriz):
        solução_x = solução_x + (0,)
        solução_final = solução_final + (0,)
        i += 1

    # Fazer o método de Jacobi até se verificar convergência.
    while not verifica_convergencia(matriz, vetor_constantes, solução_x, precisão):             
        solução_x = list(solução_x)
        solução_final = list(solução_final)  
        i = 0
        while i < (len(solução_x)):
           solução_final[i] = solução_x[i] + (vetor_constantes[i] - (produto_interno(matriz[i], solução_x))) / matriz[i][i]
           i += 1
        solução_x = tuple (solução_final)
    
    return solução_x

