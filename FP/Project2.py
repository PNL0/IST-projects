# TAD Gerador
# Representação interna: {gerador} --> {lista}
def cria_gerador(b, s):
    '''
    Esta função recebe um inteiro b correspondente ao número de bits
    do gerador e um inteiro positivo s correspondente à seed ou estado inicial,
    e devolve o gerador correspondente.
    O construtor verifica a validade dos seus argumentos.

    cria_gerador : {int} x {int} --> {gerador}
    '''
    if not isinstance(b, int) or b not in (32, 64):
        raise ValueError ("cria_gerador: argumentos invalidos")

    if not isinstance(s, int) or s <= 0:
        raise ValueError ("cria_gerador: argumentos invalidos")

    # Num gerador de 32 bits, a seed não pode ser maior que 2**32.
    if b == 32:
        if s > 2**32:
            raise ValueError ("cria_gerador: argumentos invalidos")
    # Num gerador de 64 bits, a seed não pode ser maior que 2**64.
    elif b == 64:
        if s > 2**64:
            raise ValueError ("cria_gerador: argumentos invalidos")

    return [b, s]

def cria_copia_gerador(g):
    '''
    Esta função recebe um gerador e devolve uma cópia nova do gerador.

    cria_copia_gerador: {gerador} --> {gerador}
    '''
    return g.copy()

def obtem_estado(g):
    '''
    Esta função devolve o estado atual do gerador g sem o alterar.

    obtem_estado: {gerador} --> {int}
    '''
    return g[1]

def define_estado(g, s):
    '''
    Esta função define o novo valor do estado do gerador g como sendo s,
    e devolve s.

    define_estado: {gerador} x {int} --> {int}
    '''
    g[1] = s
    return s

def atualiza_estado(g):
    '''
    Esta função atualiza o estado do gerador g de acordo com o algoritmo
    xorshift de geração de números pseudoaleatórios, e devolve-o.

    atualiza_estado: {gerador} --> {int}
    '''
    b = g[0]
    s = g[1]

    # O gerador de 32 bits utiliza valores (13, 17, 5) para os deslocamentos.
    # De modo a imitar o comportamento do gerador de 32 bits é preciso aplicar
    # um and bit-a-bit com o valor 0xFFFFFFFF após cada deslocamento à esquerda.
    if b == 32:
        s ^= (s << 13) & 0xFFFFFFFF
        s ^= (s >> 17) & 0xFFFFFFFF
        s ^= (s << 5) & 0xFFFFFFFF

    # O gerador de 64 bits utiliza valores (13, 7, 17) para os deslocamentos.
    # De modo a imitar o comportamento do gerador de 64 bits é preciso aplicar
    # um and bit-a-bit com o valor 0xFFFFFFFFFFFFFFFF
    # após cada deslocamento à esquerda.
    elif b == 64:
        s ^= (s << 13) & 0xFFFFFFFFFFFFFFFF
        s ^= (s >> 7) & 0xFFFFFFFFFFFFFFFF
        s ^= (s << 17) & 0xFFFFFFFFFFFFFFFF

    return define_estado(g, s)

def eh_gerador(arg):
    '''
    Esta função devolve True caso o seu argumento seja um TAD gerador
    e False caso contrário.

    eh_gerador : {universal} --> {booleano}
    '''
    # Para ser gerador, arg = [bits, seed]

    if not isinstance(arg, list) or len(arg) != 2:
        return False

    if not isinstance(arg[0], int) or arg[0] not in (32, 64):
        return False

    if not isinstance(arg[1], int) or arg[1] <= 0:
        return False

    # Num gerador de 32 bits, a seed não pode ser maior que 2**32.
    if arg[0] == 32:
        if arg[1] > 2**32:
            return False

    # Num gerador de 64 bits, a seed não pode ser maior que 2**64.
    elif arg[0] == 64:
        if arg[1] > 2**64:
            return False

    return True

def geradores_iguais(g1, g2):
    '''
    Esta função devolve True apenas se g1 e g2 são geradores e são iguais.

    geradores_iguais: {gerador} x {gerador} --> {booleano}
    '''
    return eh_gerador(g1) and eh_gerador(g2) and g1 == g2

def gerador_para_str(g):
    '''
    Esta função devolve a cadeia de carateres que representa o seu argumento.

    gerador_para_str : {gerador} --> {str}
    '''
    return "xorshift{}(s={})".format(g[0],g[1])

def gera_numero_aleatorio(g, n):
    '''
    Esta função atualiza o estado do gerador g e devolve um número aleatório
    no intervalo [1, n] obtido a partir do novo estado s de g como 1 + mod(s,n),
    em que mod() corresponde à operação resto da divisão inteira.

    gera_numero_aleatorio: {gerador} x {int} --> {int}
    '''
    return 1 + atualiza_estado(g) % n

def gera_carater_aleatorio(g, c):
    '''
    Esta função atualiza o estado do gerador g e devolve um caráter aleatório
    no intervalo entre "A" e o caráter maiúsculo c.
    Este é obtido a partir do novo estado s de g como o caráter na posição
    mod(s, l) da cadeia de carateres de tamanho l formada por todos
    os carateres entre "A" e c.
    A operação mod() corresponde ao resto da divisão inteira.

    gera_carater_aleatorio: {gerador} x {str} --> {str}
    '''
    # Este ciclo faz com que a lista_caracteres apreenda todos os caracteres
    # entre "A" e c, por ordem alfabética.
    lista_caracteres = []
    for i in range (ord("A"), ord(c) + 1):
        lista_caracteres.append(chr(i))

    return lista_caracteres[atualiza_estado(g) % len(lista_caracteres)]

# TAD Coordenada
# Representação interna: {coordenada} --> {tuplo}
def cria_coordenada(col, lin):
    '''
    Esta função recebe os valores correspondentes à coluna col e linha lin
    e devolve a coordenada correspondente.
    O construtor verifica a validade dos seus argumentos.

    cria_coordenada: {str} x {int} --> {coordenada}
    '''
    if not isinstance(col, str) or len(col) != 1:
        raise ValueError ("cria_coordenada: argumentos invalidos")

    # As colunas são identificadas por uma letra maiúscula de A até Z.
    if ord(col) < ord("A") or ord(col) > ord("Z"):
        raise ValueError ("cria_coordenada: argumentos invalidos")

    # As linhas são identificadas por um número inteiro do 1 até 99, no máximo.
    if not isinstance(lin, int) or lin < 1 or lin > 99:
        raise ValueError ("cria_coordenada: argumentos invalidos")

    return (col, lin)

def obtem_coluna(c):
    """
    Esta função devolve a coluna col da coordenada c.

    obtem_coluna: {coordenada} --> {str}
    """
    return c[0]

def obtem_linha(c):
    """
    Esta função devolve a linha lin da coordenada c.

    obtem_linha: {coordenada} --> {int}
    """
    return c[1]

def eh_coordenada(arg):
    '''
    Esta função devolve True caso o seu argumento seja um TAD coordenada 
    e False caso contrário.

    eh_coordenada: {universal} --> {booleano}
    '''
    # Para ser coordenada, arg = (coluna, linha)
    if not isinstance(arg, tuple) or len(arg) != 2:
        return False

    if not isinstance(arg[0], str) or len(arg[0]) != 1:
        return False
    
    # As colunas são identificadas por uma letra maiúscula de A até Z.
    if ord(arg[0]) < ord("A") or ord(arg[0]) > ord("Z"):
        return False

    # As linhas são identificadas por um número inteiro do 1 até 99, no máximo.
    if not isinstance(arg[1], int) or arg[1] < 1 or arg[1] > 99:
        return False

    return True

def coordenadas_iguais (c1, c2):
    '''
    Esta função devolve True apenas se c1 e c2 são coordenadas e são iguais.

    coordenadas_iguais: {coordenada} x {coordenada} --> {booleano}
    '''
    return eh_coordenada(c1) and eh_coordenada(c2) and c1 == c2

def coordenada_para_str(c):
    '''
    Esta função devolve a cadeia de carateres que representa o seu argumento.

    coordenada_para_str : {coordenada} --> {str}
    '''
    if c[1] < 10:
        # As linhas de 1 a 9 são representadas como 01, 02, 03...
        return "{}".format(c[0] + "0" + str(c[1]))

    return "{}".format(c[0] + str(c[1]))

def str_para_coordenada(s):
    '''
    Esta função devolve a coordenada representada pelo seu argumento.

    str_para_coordenada: {str} --> {coordenada}
    '''
    if s[1] == "0":
        # Ex: A01 corresponde à coordenada ("A", 1), pois ("A", 01) não existe.
        return (s[0], int(s[2]))

    return (s[0], int (s[1:]))

def obtem_coordenadas_vizinhas(c):
    '''
    Esta função devolve um tuplo com as coordenadas vizinhas à coordenada c,
    começando pela coordenada na diagonal acima-esquerda de c
    e seguindo no sentido horário.

    obtem_coordenadas_vizinhas: {coordenada} --> {tuplo}
    '''
    # Coordenadas de c.
    col = obtem_coluna(c)
    lin = obtem_linha(c)

    coordenadas_vizinhas = ()

    # Este ciclo obtem as coordenadas vizinhas de c que estão na linha anterior
    # à linha onde está c.
    for coluna in range (ord(col) - 1, ord(col) + 2):

        # A linha anterior não pode ser menor que 1 e as colunas são
        # identificadas por letras maiúsculas de A até Z, no máximo.
        if (1 <= lin - 1) and (ord("A") <= coluna <= ord("Z")):
            coordenadas_vizinhas += (cria_coordenada(chr(coluna), lin - 1), )

    # É obtida a coordenada que está à direita de c, caso a coluna seguinte
    # seja uma letra maiúscula e esteja entre "A" e "Z".
    if ord("A") <= ord(col) + 1 <= ord("Z"):
        coordenadas_vizinhas += (cria_coordenada(chr(ord(col) + 1), lin), )

    # Este ciclo obtem as coordenadas vizinhas de c que estão na linha seguinte
    # à linha onde está c.
    for coluna in range (ord(col) + 1, ord(col) - 2, -1):

        # A linha seguinte não pode ser maior que 99 e as colunas são
        # identificadas por letras maiúsculas de A até Z, no máximo.
        if (lin + 1 <= 99) and (ord("A") <= coluna <= ord("Z")):
            coordenadas_vizinhas += (cria_coordenada(chr(coluna), lin + 1), )

    # É obtida a coordenada que está à esquerda de c, caso a coluna anterior
    # seja uma letra maiúscula e esteja entre "A" e "Z".
    if ord("A") <= ord(col) - 1 <= ord("Z"):
        coordenadas_vizinhas += (cria_coordenada(chr(ord(col) - 1), lin), )

    return coordenadas_vizinhas

def obtem_coordenada_aleatoria(c, g):
    '''
    Esta função recebe uma coordenada c e um TAD gerador g, e devolve uma
    coordenada gerada aleatoriamente, em que c define a maior coluna e maior
    linha possíveis. É ser gerada, em sequência, primeiro a coluna e depois
    a linha da coordenada resultado.

    obtem_coordenada_aleatoria: {coordenada} x {gerador} --> {coordenada}
    '''
    return cria_coordenada(gera_carater_aleatorio(g, obtem_coluna(c)),
           gera_numero_aleatorio(g, obtem_linha(c)))


# TAD Parcela
# Representação interna: {parcela} --> {lista}
def cria_parcela():
    '''
    Esta função devolve uma parcela tapada sem mina escondida.

    cria_parcela: {} --> {parcela}
    '''
    return ["tapadas", "não minadas"]

def cria_copia_parcela(p):
    '''
    Esta função recebe uma parcela p e devolve uma nova cópia da parcela.

    cria_copia_parcela: {parcela} --> {parcela}
    '''
    return p.copy()

def limpa_parcela(p):
    '''
    Esta função modifica destrutivamente a parcela p modificando o seu estado
    para limpa, e devolve a própria parcela.

    limpa_parcela: {parcela} --> {parcela}
    '''
    p[0] = "limpas"
    return p

def marca_parcela(p):
    '''
    Esta função modifica destrutivamente a parcela p modificando o seu estado
    para marcada com uma bandeira, e devolve a própria parcela.

    marca_parcela: {parcela} --> {parcela}
    '''
    p[0] = "marcadas"
    return p

def desmarca_parcela(p):
    '''
    Esta função modifica destrutivamente a parcela p modificando o seu estado
    para tapada, e devolve a própria parcela.

    desmarca_parcela: {parcela} --> {parcela}
    '''
    p[0] = "tapadas"
    return p

def esconde_mina(p):
    '''
    Esta função modifica destrutivamente a parcela p escondendo uma mina
    na parcela, e devolve a própria parcela.

    esconde_mina: {parcela} --> {parcela}
    '''
    p[1] = "minadas"
    return p

def eh_parcela(arg):
    '''
    Esta função devolve True caso o seu argumento seja um TAD parcela
    e False caso contrário.

    eh_parcela: {universal} --> {booleano}
    '''
    # A parcela tem de ser uma lista com 2 elementos.
    if not isinstance(arg, list) or len(arg) != 2:
        return False

    # A parcela só pode estar tapada, limpa ou marcada.
    if (arg[0] != "tapadas" and arg[0] != "limpas" and arg[0] != "marcadas"):
        return False

    # A parcela pode ter ou não uma mina escondida.
    if (arg[1] != "não minadas" and arg[1] != "minadas"):
        return False

    return True

def eh_parcela_tapada(p):
    '''
    Esta função devolve True caso a parcela p se encontre tapada
    e False caso contrário.

    eh_parcela_tapada: {parcela} --> {booleano}
    '''
    return p[0] == "tapadas"

def eh_parcela_marcada(p):
    '''
    Esta função devolve True caso a parcela p se encontre marcada
    com uma bandeira e False caso contrário.

    eh_parcela_marcada: {parcela} --> {booleano}
    '''
    return p[0] == "marcadas"

def eh_parcela_limpa(p):
    '''
    Esta função devolve True caso a parcela p se encontre limpa
    e False caso contrário.

    eh_parcela_limpa: {parcela} --> {booleano}
    '''
    return p[0] == "limpas"

def eh_parcela_minada(p):
    '''
    Esta função devolve True caso a parcela p esconda uma mina
    e False caso contrário.

    eh_parcela_minada: {parcela} --> {booleano}
    '''
    return p[1] == "minadas"

def parcelas_iguais(p1, p2):
    '''
    Esta função devolve True apenas se p1 e p2 são parcelas e são iguais.

    parcelas_iguais: {parcela} x {parcela} --> {booleano}
    '''
    if not eh_parcela(p1) or not eh_parcela(p2):
        return False

    return p1 == p2

def parcela_para_str(p):
    '''
    Esta função devolve a cadeia de caracteres que representa a parcela
    em função do seu estado: parcelas tapadas ("#"), parcelas marcadas ("@"),
    parcelas limpas sem mina ("?") e parcelas limpas com mina ("X").

    parcela_para_str : {parcela} --> {str}
    '''
    if p[0] == "tapadas":
        return "#"

    if p[0] == "marcadas":
        return "@"

    if p[0] == "limpas":
        if p[1] == "não minadas":
            return "?"
        return "X"

def alterna_bandeira(p):
    '''
    Esta função recebe uma parcela p e modifica-a destrutivamente
    da seguinte forma: desmarca se estiver marcada e marca se estiver tapada,
    devolvendo True.
    Em qualquer outro caso, não modifica a parcela e devolve False.

    alterna_bandeira: {parcela} --> {booleano}
    '''
    if eh_parcela_marcada(p):
        desmarca_parcela(p)
        return True

    if eh_parcela_tapada(p):
        marca_parcela(p)
        return True

    return False

# TAD campo
# Representação interna: {campo} --> {lista}
def cria_campo(c, l):
    '''
    Esta função recebe uma cadeia de carateres e um inteiro correspondentes
    à última coluna e à última linha de um campo de minas, e devolve o campo
    do tamanho pretendido formado por parcelas tapadas sem minas.
    O construtor verifica a validade dos seus argumentos.

    cria_campo: {str} x {int} --> {campo}
    '''
    if not isinstance(c, str) or len(c) != 1:
        raise ValueError ("cria_campo: argumentos invalidos")

    # As colunas são identificadas por uma letra maiúscula de A até Z.
    if ord(c) < ord("A") or ord("Z") < ord (c):
        raise ValueError ("cria_campo: argumentos invalidos")

    # As linhas são identificadas por um número inteiro do 1 até 99, no máximo.
    if not isinstance(l, int) or l < 1 or l > 99:
        raise ValueError ("cria_campo: argumentos invalidos")

    campo = []
    # Para todas as linhas de 1 até l.
    for linha in range (1, l + 1):

        # Para todas as colunas de "A" até c.
        for coluna in range (ord("A"), ord(c) + 1):

            # O campo vai apreendendo listas com coordenada e a parcela respetiva.
            campo.append([cria_coordenada(chr(coluna), linha), cria_parcela()])

    return campo

def cria_copia_campo(m):
    '''
    Esta função recebe um campo e devolve uma nova cópia do campo.

    cria_copia_campo: {campo} --> {campo}
    '''
    copia_campo = []
    for elemento in m:
        # elemento = [coordenada, parcela]
        copia_campo.append([elemento[0], cria_copia_parcela(elemento[1])])

    return copia_campo

def obtem_ultima_coluna(m):
    '''
    Esta função devolve a cadeia de caracteres que corresponde
    à última coluna do campo de minas.

    obtem_ultima_coluna: {campo} --> {str}
    '''
    # m[-1][0] corresponde ao tuplo das coordenadas da ultima sublista do campo.
    return obtem_coluna(m[-1][0])

def obtem_ultima_linha(m):
    '''
    Esta função devolve o valor inteiro que corresponde à última linha
    do campo de minas.

    obtem_ultima_linha: {campo} --> {int}
    '''
    # m[-1][0] corresponde ao tuplo das coordenadas da ultima sublista do campo.
    return obtem_linha(m[-1][0])

def obtem_parcela(m, c):
    '''
    Esta função devolve a parcela do campo m que se encontra na coordenada c.

    obtem_parcela: {campo} x {coordenada} --> {parcela}
    '''
    for elemento in m:
        # elemento = [coordenada, parcela]
        if coordenadas_iguais(elemento[0], c):
            return elemento[1]

def obtem_coordenadas(m, s):
    '''
    Esta função devolve o tuplo formado pelas coordenadas ordenadas
    em ordem ascendente de esquerda à direita e de cima a baixo das parcelas
    dependendo do valor de s: "limpas" para as parcelas limpas, "tapadas" para
    as parcelas tapadas, "marcadas" para as parcelas marcadas,
    e "minadas" para as parcelas que escondem minas.

    obtem_coordenadas: {campo} x {str} --> {tuplo}
    '''
    res = ()
    if s == "limpas":
        for elemento in m:
            # elemento = [coordenada, parcela]
            if eh_parcela_limpa(elemento[1]):
                res += (elemento[0],)

    if s == "tapadas":
        for elemento in m:
            if eh_parcela_tapada(elemento[1]):
                res += (elemento[0],)

    if s == "marcadas":
        for elemento in m:
            if eh_parcela_marcada(elemento[1]):
                res += (elemento[0],)

    if s == "minadas":
        for elemento in m:
            if eh_parcela_minada(elemento[1]):
                res += (elemento[0],)             
    return res

def obtem_numero_minas_vizinhas(m, c):
    '''
    Esta função devolve o número de parcelas vizinhas da parcela
    na coordenada c que escondem uma mina.

    obtem_numero_minas_vizinhas: {campo} x {coordenada} --> {int}
    '''
    coordenadas_vizinhas = obtem_coordenadas_vizinhas(c)
    coordenadas_minadas = obtem_coordenadas(m, "minadas")
    res = 0
    for coordenada in coordenadas_vizinhas:
        if coordenada in coordenadas_minadas:
            res += 1
    return res

def eh_campo(arg):
    '''
    Esta função devolve True caso o seu argumento seja um TAD campo
    e False caso contrário.

    eh_campo: {universal} --> {booleano}
    '''
    # O campo tem de ser uma lista não vazia.
    if not isinstance(arg, list) or len(arg) <= 0:
        return False

    for elemento in arg:
        # elemento = [coordenada, parcela]

        if not isinstance(elemento, list) or len(elemento) != 2:
            return False

        if not eh_coordenada(elemento[0]):
            return False

        if not eh_parcela(elemento[1]):
            return False

    return True

def eh_coordenada_do_campo(m, c):
    '''
    Esta função devolve True se c é uma coordenada válida dentro do campo m.

    eh_coordenada_do_campo: {campo} x {coordenada} --> {booleano}
    '''
    for elemento in m:
        # elemento = [coordenada, parcela]
        if coordenadas_iguais(elemento[0], c):
            return True

    return False

def campos_iguais (m1, m2):
    '''
    Esta função devolve True apenas se m1 e m2 forem campos e forem iguais.

    campos_iguais: {campo} x {campo} --> {booleano}
    '''
    if not eh_campo(m1) or not eh_campo(m2):
        return False

    # m1 e m2 têm de ter exatamente o mesmo número de colunas.
    if obtem_ultima_coluna(m1) != obtem_ultima_coluna(m2):
        return False

    # m1 e m2 têm de ter exatamente o mesmo número de linhas.
    if obtem_ultima_linha(m1) != obtem_ultima_linha(m2):
        return False

    for i in range(len(m1)):
        # m[i] = [coordenada, parcela]

        if not coordenadas_iguais(m1[i][0], m2[i][0]):
            return False
            
        if not parcelas_iguais(m1[i][1], m2[i][1]):
            return False 

    return True

def campo_para_str(m):
    '''
    Esta função devolve uma cadeia de caracteres que representa o campo de minas.

    campo_para_str : {campo} --> {str}
    '''
    campo = "   "
    colunas = ""
    for elemento in m:
        # elemento = [coordenada, parcela]
        if obtem_coluna(elemento[0]) not in colunas:
            colunas += obtem_coluna(elemento[0])
    campo += colunas

    campo += "\n  +"
    for coluna in colunas:
        campo += "-"
    campo += "+\n"

    linha = 1
    while linha <= obtem_ultima_linha(m):
        linha_para_adicionar = ""
        if linha < 10:
            # Números das linhas entre 1 e 9 são representados como 01,02,03...
            linha_para_adicionar += "0{}|".format(linha)
        else:
            linha_para_adicionar += "{}|".format(linha)

        for coluna in colunas:
            coordenada = cria_coordenada(coluna, linha)
            parcela = obtem_parcela(m, coordenada)

            if eh_parcela_limpa(parcela):

                if eh_parcela_minada(parcela):
                    linha_para_adicionar += parcela_para_str(parcela)

                # Parcelas limpas sem minas vizinhas são representadas por " "
                elif obtem_numero_minas_vizinhas(m, coordenada) == 0:
                    linha_para_adicionar += " "

                # Parcelas limpas com minas vizinhas são representadas
                # pelo número de minas vizinhas que têm.
                else:
                    linha_para_adicionar += "{}".format(
                    obtem_numero_minas_vizinhas(m, coordenada))

            else:
                linha_para_adicionar += parcela_para_str(parcela)

        linha_para_adicionar += "|\n"
        # Em cada ciclo é construida uma linha do campo.
        campo += linha_para_adicionar
        linha += 1

    campo += "  +"
    for coluna in colunas:
        campo += "-"
    campo += "+"

    return campo

def coloca_minas(m, c, g, n):
    '''
    Esta função modifica destrutivamente o campo m escondendo n minas
    em parcelas dentro do campo. As n coordenadas são geradas em sequência
    utilizando o gerador g, de modo a que não coincidam com a coordenada c
    nem com nenhuma parcela vizinha desta, nem se sobreponham com minas
    colocadas anteriormente.

    coloca_minas: {campo} x {coordenada} x {gerador} x {int} --> {campo}
    '''
    while len(obtem_coordenadas(m, "minadas")) < n:

        coordenada_aleatória = obtem_coordenada_aleatoria(
            cria_coordenada(obtem_ultima_coluna(m), obtem_ultima_linha(m)), g)

        if coordenada_aleatória != c:
            if coordenada_aleatória not in obtem_coordenadas_vizinhas(c):
                if coordenada_aleatória not in obtem_coordenadas(m, "minadas"):
                    esconde_mina(obtem_parcela(m, coordenada_aleatória))

    return m

def limpa_campo(m, c):
    '''
    Esta função modifica destrutivamente o campo limpando a parcela
    na coordenada c e o devolvendo-a. Se não houver nenhuma mina vizinha
    escondida, limpa iterativamente todas as parcelas vizinhas tapadas.
    Caso a parcela se encontre já limpa, a operação não tem efeito.

    limpa_campo: {campo} x {coordenada} --> {campo}
    '''
    parcela_de_c = obtem_parcela(m, c)
    limpa_parcela(parcela_de_c)

    if not eh_parcela_minada(parcela_de_c):
        
        coordenadas_limpas = (c, )
        i = 0
        while i < (len(coordenadas_limpas)):

            if obtem_numero_minas_vizinhas (m, coordenadas_limpas[i]) == 0:
                
                for coordenada in obtem_coordenadas_vizinhas(coordenadas_limpas[i]):

                    if eh_coordenada_do_campo(m, coordenada):
                        if eh_parcela_tapada(obtem_parcela(m, coordenada)):
                            if coordenada not in coordenadas_limpas:

                                limpa_parcela(obtem_parcela(m, coordenada))
                                coordenadas_limpas += (coordenada, )
            i += 1
    return m

# Funções adicionais
def jogo_ganho(m):
    '''
    Esta função recebe um campo do jogo das minas e devolve True se todas as
    parcelas sem minas se encontram limpas, ou False caso contrário.

    jogo_ganho: {campo} --> {booleano}
    '''
    coordenadas_tapadas = obtem_coordenadas(m, "tapadas")
    coordenadas_marcadas = obtem_coordenadas(m, "marcadas")

    coordenadas_escondidas = coordenadas_marcadas + coordenadas_tapadas

    for coordenada in coordenadas_escondidas:
        if coordenada not in obtem_coordenadas(m, "minadas"):
            return False

    return True

def coordenada_aux (campo, coordenada):
    '''
    Esta função auxiliar serve para verificar a validade de uma coordenada
    dada pelo jogador.

    coordenada_aux: {campo} x {str} --> {booleano}
    '''
    if len(coordenada) != 3 or not coordenada[1:].isnumeric():
        return False

    if not eh_coordenada(str_para_coordenada(coordenada)):
        return False

    if not eh_coordenada_do_campo(campo, str_para_coordenada (coordenada)):
        return False

    return True

def turno_jogador(m):
    '''
    Esta função recebe um campo de minas e oferece ao jogador a opção de
    escolher uma ação e uma coordenada. A função modifica destrutivamente
    o campo de acordo com ação escolhida, devolvendo False caso o jogador
    tenha limpo uma parcela que continha uma mina, ou True caso contrário.
    A mensagem "Escolha uma ação, [L]impar ou [M]arcar:" é repetida até
    o jogador introduzir uma ação válida ("L" ou "M"). O mesmo se verifica
    para a coordenada onde a mensagem "Escolha uma coordenada:" é repetida
    até que o jogador introduza uma coordenada válida.

    turno_jogador: {campo} --> {booleano}
    '''
    try:
        ação = input("Escolha uma ação, [L]impar ou [M]arcar:")
        while ação not in ("L", "M"):
            ação = input("Escolha uma ação, [L]impar ou [M]arcar:")
    except EOFError:
        ação = ""

    try:
        coordenada = input("Escolha uma coordenada:")
        while not coordenada_aux(m, coordenada):
            coordenada = input("Escolha uma coordenada:")
    except EOFError:
        coordenada = "0"

    if ação == "M":
        alterna_bandeira(obtem_parcela(m, str_para_coordenada(coordenada)))
        return True

    if ação == "L":
        limpa_campo(m, str_para_coordenada(coordenada))
        return str_para_coordenada(coordenada) not in obtem_coordenadas(m,"minadas")

def minas(c, l, n, d, s):
    '''
    Esta é a função principal que permite jogar ao jogo das minas.
    A função recebe uma cadeia de carateres e 4 valores inteiros correspondentes,
    respetivamente, a: última coluna c; última linha l; número de parcelas com
    minas n; dimensão do gerador de números d; e estado inicial ou seed s para
    a geração de números aleatórios. A função devolve True se o jogador
    conseguir ganhar o jogo, ou False caso contrário.
    A função deve verificar a validade dos seus argumentos.

    minas: {str} x {int} x {int} x {int} x {int} --> {booleano}
    '''
    if not isinstance(c, str) or len(c) != 1:
        raise ValueError ("minas: argumentos invalidos")

    # As colunas são identificadas por letras maiúsculas de A até Z, no máximo.
    if ord(c) < ord("A") or ord(c) > ord("Z"):
        raise ValueError ("minas: argumentos invalidos")

    # As linhas são identificadas por um número inteiro do 1 até 99, no máximo.
    if not isinstance(l, int) or l < 1 or l > 99:
        raise ValueError ("minas: argumentos invalidos")

    # O número de parcelas com mina tem de ser um inteiro positivo.
    if not isinstance(n, int) or n <= 0:
        raise ValueError ("minas: argumentos invalidos")

    # O número de bits tem de ser um inteiro igual a 32 ou 64.
    if not isinstance(d, int) or d not in (32, 64):
        raise ValueError ("minas: argumentos invalidos")

    # A seed é um inteiro positivo.
    if not isinstance(s, int) or s <= 0:
        raise ValueError ("minas: argumentos invalidos")

    # Num gerador de 32 bits, a seed não pode ser maior que 2**32.
    if d == 32:
        if s > 2**32:
            raise ValueError ("minas: argumentos invalidos")

    # Num gerador de 64 bits, a seed não pode ser maior que 2**64.
    elif d == 64:
        if s > 2**64:
            raise ValueError ("minas: argumentos invalidos")

    campo = cria_campo(c, l)

    # O campo tem dimensões mínimas dependendo do número de minas
    # que se pretende colocar.
    if ((ord(c) - ord("A") + 1) * l - 9) < n:
        raise ValueError ("minas: argumentos invalidos")

    parcelas_marcadas = 0

    print("   [Bandeiras {}/{}]\n{}".format(
        parcelas_marcadas, n, campo_para_str(campo)))

    coordenada = input("Escolha uma coordenada:")
    while not coordenada_aux(campo, coordenada):
        coordenada = input("Escolha uma coordenada:")

    campo = coloca_minas(campo, str_para_coordenada(coordenada),cria_gerador(d,s),n)
    campo = limpa_campo(campo, str_para_coordenada(coordenada))

    print("   [Bandeiras {}/{}]\n{}".format(
        parcelas_marcadas, n, campo_para_str(campo)))

    while not jogo_ganho(campo):

        # Se esta condição se verificar quer dizer que o jogador perdeu.
        if not turno_jogador(campo):

            parcelas_marcadas = len(obtem_coordenadas(campo, "marcadas"))

            print("   [Bandeiras {}/{}]\n{}".format(
                parcelas_marcadas, n, campo_para_str(campo)))

            print("BOOOOOOOM!!!")
            return False

        parcelas_marcadas = len(obtem_coordenadas(campo, "marcadas"))

        print("   [Bandeiras {}/{}]\n{}".format(
            parcelas_marcadas, n, campo_para_str(campo)))

    print("VITORIA!!!")
    return True
