% 106154 - Pedro Nunes Leal

:- set_prolog_flag(answer_write_options,[max_depth(0)]). % para listas completas
:- [dados], [keywords]. % ficheiros a importar.

% 3.1 Qualidade dos dados

% Este predicado devolve uma lista de IDs de eventos sem sala.
eventosSemSalas(EventosSemSala):-
    findall(ID,evento(ID,_,_,_,semSala),EventosSemSala).

% Este predicado encontra e devolve uma lista de IDs de eventos sem sala
% que decorrem num determinado dia da semana.
eventosSemSalasDiaSemana(DiaDaSemana,EventosSemSala):-
    findall(ID,(evento(ID,_,_,_,semSala),horario(ID,DiaDaSemana,_,_,_,_)),EventosSemSala).

% Este predicado auxiliar serve para obter os eventos sem sala
% semestrais, que decorrem durante o periodo.
eventosSemSalasPeriodo_aux(Periodo,EventosSemSala_semestrais):-
   ((Periodo == p1 ; Periodo == p2),
   findall(ID,(evento(ID,_,_,_,semSala),horario(ID,_,_,_,_,p1_2)),
           EventosSemSala_semestrais));

   ((Periodo == p3 ; Periodo == p4),
   findall(ID,(evento(ID,_,_,_,semSala),horario(ID,_,_,_,_,p3_4)),
           EventosSemSala_semestrais)).

% Este predicado encontra e devolve uma lista de IDs de eventos sem
% sala, que decorrem durante um ou mais periodos.
eventosSemSalasPeriodo([Periodo|R],EventosSemSala):-
   eventosSemSalasPeriodo_aux(Periodo, EventosSemSala_semestrais),
   findall(ID,(evento(ID,_,_,_,semSala),horario(ID,_,_,_,_,Periodo)),EventosSemSala_P_aux),
   append(EventosSemSala_semestrais, EventosSemSala_P_aux, EventosSemSala_Periodo),

   eventosSemSalasPeriodo(R, EventosSemSala_R),
   append(EventosSemSala_Periodo, EventosSemSala_R, EventosSemSala_NaoOrdenados),
   setof(ID, member(ID,EventosSemSala_NaoOrdenados),EventosSemSala),!; EventosSemSala = [].
eventosSemSalasPeriodo([],[]).


% 3.2 Pesquisas simples

% Este predicado auxiliar serve para juntar duas listas.
juntalistas([P|R],L1,[P|L2]):- juntalistas(R,L1,L2).
juntalistas([],L,L).

% Este predicado auxiliar serve para partir uma lista em duas em que uma
% parte eh menor que o numero(N) e a outra maior.
partelistas([P|R],N,[P|R1],L2):- P < N, partelistas(R,N,R1,L2).
partelistas([P|R],N, L1,[P|R2]):- P >= N, partelistas(R,N,L1,R2).
partelistas([ ],_,[ ],[ ]).

% Este predicado auxiliar serve para ordenar uma lista.
ordenalistas([P|R],L):-
    partelistas(R,P,L_Menores,L_Maiores),
    ordenalistas(L_Menores,L_MenoresOrdenada),
    ordenalistas(L_Maiores, L_MaioresOrdenada),
    juntalistas(L_MenoresOrdenada,[P|L_MaioresOrdenada],L).
ordenalistas([ ],[ ]).

% Este predicado auxiliar serve para verificar se o ID,
% corresponde a um evento semestral que decorra no Periodo.
verificaEventos_Semetrais(ID,Periodo,_):-
    (Periodo == p1 ; Periodo == p2), horario(ID,_,_,_,_,p1_2);
    (Periodo == p3 ; Periodo == p4), horario(ID,_,_,_,_,p3_4).

% Este predicado encontra e devolve a lista de IDs dos eventos de
% ListaEventos que ocorrem no Periodo.
organizaEventos(ListaEventos,Periodo,[P|R_aux]):-
    ordenalistas(ListaEventos,[P|R]),
    (horario(P,_,_,_,_,Periodo); verificaEventos_Semetrais(P,Periodo,_)),
    organizaEventos(R,Periodo,R_aux).

organizaEventos(ListaEventos,Periodo,EventosNoPeriodo):-
    ordenalistas(ListaEventos,[_|R]),
    organizaEventos(R,Periodo,EventosNoPeriodo).
organizaEventos([],_,[]).

% Este predicado devolve a lista de IDs dos eventos que tem duracaoo
% menor ou igual a Duracao.
eventosMenoresQue(Duracao,ListaEventosMenoresQue):-
    findall(ID,(horario(ID,_,_,_,Duracao_ID,_),Duracao_ID =< Duracao),
            ListaEventosMenoresQue).

% Este predicado verifica se o evento identificado por ID tem duracao
% igual ou menor a Duracao.
eventosMenoresQueBool(ID,Duracao):-
    horario(ID,_,_,_,Duracao_ID,_), Duracao_ID =< Duracao.

% Este predicado encontra e devolve a lista de disciplinas de um
% determinado curso.
procuraDisciplinas(Curso, ListaDisciplinas):-
    findall(Disciplina,(evento(ID,Disciplina,_,_,_),turno(ID,Curso,_,_)),
                        ListaDisciplinas_aux),
    setof(Disciplina, member(Disciplina,ListaDisciplinas_aux),ListaDisciplinas).

% Este predicado encontra e devolve uma lista com duas listas.
% A primeira contem as disciplinas de ListaDisciplinas do Curso que
% ocorrem no primeiro semestre e a segunda as que ocorrem no segundo
% semestre.
organizaDisciplinas([Disciplina|R],Curso,[[Disciplina|R1],R_aux]):-
    evento(ID,Disciplina,_,_,_), turno(ID,Curso,_,_), horario(ID,_,_,_,_,Periodo),
    (Periodo == p1; Periodo == p2; Periodo == p1_2),
    organizaDisciplinas(R,Curso,[R1,R_aux]).

organizaDisciplinas([Disciplina|R],Curso,[R_aux,[Disciplina|R1]]):-
    evento(ID,Disciplina,_,_,_), turno(ID,Curso,_,_), horario(ID,_,_,_,_,Periodo),
    (Periodo == p3; Periodo == p4; Periodo == p3_4),
    organizaDisciplinas(R,Curso,[R_aux,R1]).
organizaDisciplinas([],_,[[],[]]).

% Este predicado auxiliar serve para somar todos os elementos de uma
% lista.
soma([P|R],Res):-
    soma(R,Res_aux), Res is P + Res_aux.
soma([],0).

% Este predicado auxiliar serve para obter a lista de IDs dos eventos
% semestrais que decorrem durante o Periodo, de um Curso e Ano.
horasCurso_aux(Periodo,Curso,Ano,Lista_ID_semestrais):-
    (((Periodo == p1 ; Periodo == p2),
    findall(ID,(turno(ID,Curso,Ano,_),horario(ID,_,_,_,_,p1_2)),Lista_ID_semestrais_aux));
    ((Periodo == p3 ; Periodo == p4),
    findall(ID,(turno(ID,Curso,Ano,_),horario(ID,_,_,_,_,p3_4)),Lista_ID_semestrais_aux))),

    (setof(ID, member(ID,Lista_ID_semestrais_aux),Lista_ID_semestrais),! ;
    Lista_ID_semestrais = []).

% Este predicado devolve o numero de horas total dos eventos associados
% ao Curso, num determinado Ano e Periodo.
horasCurso(Periodo,Curso,Ano,TotalHoras):-
  (findall(ID,(horario(ID,_,_,_,_,Periodo),turno(ID,Curso,Ano,_)),Lista_ID_aux),
   setof(ID, member(ID,Lista_ID_aux),Lista_ID),
   horasCurso_aux(Periodo,Curso,Ano,Lista_ID_semestrais),
   append(Lista_ID, Lista_ID_semestrais, Lista_Final),

   findall(Duracao,(member(ID, Lista_Final), horario(ID,_,_,_,Duracao,_)),Lista_Duracoes),
   soma(Lista_Duracoes,TotalHoras)),! ; TotalHoras is 0.

% Este predicado encontra e devolve uma lista de tuplos na forma
% (Ano,Periodo,NumHoras), em que NumHoras eh o total de horas associadas
% ao Curso, num determinado Ano e Periodo.
evolucaoHorasCurso(Curso,Evolucao):-
    Lista_Anos = [1,2,3], Lista_Periodos = [p1,p2,p3,p4],
    findall((Ano,Periodo,NumHoras),(member(Ano,Lista_Anos),member(Periodo,Lista_Periodos),
            horasCurso(Periodo,Curso,Ano,NumHoras)),Evolucao).


% 3.3 Ocupacoes criticas de salas

% Este predicado encontra e devolve o numero de horas sobrepostas entre
% o evento que tem inicio em HoraInicioEvento e fim em HoraFimEvento, e
% o slot que tem inicio em HoraInicioDada e fim em HoraFimDada.
ocupaSlot(HoraInicioDada,HoraFimDada,HoraInicioEvento,HoraFimEvento,Horas):-

    % caso 1: o evento fica totalmente contido no slot.
    (HoraInicioDada =< HoraInicioEvento, HoraFimEvento =< HoraFimDada,
     Horas is HoraFimEvento - HoraInicioEvento, Horas > 0);

    % caso 2: o evento contem totalmente o slot.
    (HoraInicioEvento < HoraInicioDada,  HoraFimDada < HoraFimEvento,
     Horas is HoraFimDada - HoraInicioDada, Horas > 0);

    % caso 3: a sobreposicao eh no inicio do evento.
    (HoraInicioDada =< HoraInicioEvento, HoraFimDada < HoraFimEvento,
     Horas is HoraFimDada - HoraInicioEvento, Horas > 0);

    % caso 4: a sobreposicao eh no fim do evento.
    (HoraInicioEvento < HoraInicioDada, HoraFimEvento =< HoraFimDada,
     Horas is HoraFimEvento - HoraInicioDada, Horas > 0).

    % caso 5: nao ha sobreposicao e por isso o predicado falha.

% Este predicado auxiliar serve para obter uma lista das horas ocupadas
% por cada uma das disciplinas semestrais, numa dada sala,
% periodo e intervalo de tempo.
numHorasOcupadas_aux(Periodo,TipoSala,DiaSemana,HoraInicio,HoraFim,SomaHoras_2):-
    salas(TipoSala,Lista_Salas),
    ((Periodo == p1; Periodo == p2),
    findall(Horas,(evento(ID,_,_,_,Sala), member(Sala,Lista_Salas),
            horario(ID,DiaSemana,HoraInicioEvento,HoraFimEvento,_,p1_2),
            ocupaSlot(HoraInicio,HoraFim,HoraInicioEvento,HoraFimEvento,Horas)),
            Lista_Horas),
    soma(Lista_Horas,SomaHoras_2)),! ;

    (salas(TipoSala,Lista_Salas),
    findall(Horas,(evento(ID,_,_,_,Sala), member(Sala,Lista_Salas),
            horario(ID,DiaSemana,HoraInicioEvento,HoraFimEvento,_,p3_4),
            ocupaSlot(HoraInicio,HoraFim,HoraInicioEvento,HoraFimEvento,Horas)),
            Lista_Horas),
     soma(Lista_Horas,SomaHoras_2)).

% Este predicado encontra e devolve o numero de horas ocupadas nas salas
% do tipo TipoSala, no intervalo de tempo definido entre HoraInicio e
% HoraFim, no dia da semana DiaSemana, e no periodo Periodo.
numHorasOcupadas(Periodo,TipoSala,DiaSemana,HoraInicio,HoraFim,SomaHoras):-
    salas(TipoSala,Lista_Salas),
    findall(Horas,(evento(ID,_,_,_,Sala), member(Sala,Lista_Salas),
            horario(ID,DiaSemana,HoraInicioEvento,HoraFimEvento,_,Periodo),
            ocupaSlot(HoraInicio,HoraFim,HoraInicioEvento,HoraFimEvento,Horas)),
            Lista_Horas),
    soma(Lista_Horas,SomaHoras_1),
    numHorasOcupadas_aux(Periodo,TipoSala,DiaSemana,HoraInicio,HoraFim,SomaHoras_2),
    SomaHoras is SomaHoras_1 + SomaHoras_2.

% Este predicado encontra e devolve o numero de horas possiveis de ser
% ocupadas por salas do tipo TipoSala, no intervalo de tempo definido
% entre HoraInicio e HoraFim.
ocupacaoMax(TipoSala,HoraInicio,HoraFim,Max):-
    salas(TipoSala,Lista_Salas),
    length(Lista_Salas,Numero_Salas),
    Max is (HoraFim - HoraInicio) * Numero_Salas.

% Este predicado eh verdade se Percentagem for a divisao de SomaHoras
% por Max, multiplicada por 100.
percentagem(SomaHoras,Max,Percentagem):-
    Percentagem is(SomaHoras / Max) * 100.

% Este predicado encontra e devolve uma lista de tuplos do tipo
% casosCriticos(DiaSemana, TipoSala,Percentagem) em que DiaSemana,
% TipoSala e Percentagem sao, respectivamente, um dia da semana, um tipo
% de sala e a sua percentagem de ocupacao, no intervalo de tempo entre
% HoraInicio e HoraFim, e supondo que a percentagem de ocupacao relativa
% a esses elementos esta acima de um dado valor critico (Threshold).
ocupacaoCritica(HoraInicio,HoraFim,Threshold,Resultados):-
    Lista_DiaSemana = [quarta-feira,quinta-feira,segunda-feira,sexta-feira,terca-feira],
    Lista_TipoSala  = [grandesAnfiteatros,labsElectro,labsFisica,labsJogos,labsPC,
                       labsQuimica,labsRedes,pequenosAnfiteatros,salasAula,videoConf],
    Lista_Periodos  = [p1,p2,p3,p4],

    findall(casosCriticos(DiaSemana,TipoSala,Percentagem_Arredondada),
            (member(DiaSemana,Lista_DiaSemana),member(TipoSala,Lista_TipoSala),
             member(Periodo,Lista_Periodos),
             numHorasOcupadas(Periodo,TipoSala,DiaSemana,HoraInicio,HoraFim,SomaHoras),
             ocupacaoMax(TipoSala,HoraInicio,HoraFim,Max),
             percentagem(SomaHoras,Max,Percentagem), Percentagem > Threshold,
             Percentagem_Arredondada is ceiling(Percentagem)), Resultados_aux),

    setof(casosCriticos(DiaSemana,TipoSala,Percentagem_Arredondada),
          member(casosCriticos(DiaSemana,TipoSala,Percentagem_Arredondada),
                 Resultados_aux),Resultados).


% 3.4 And now for something completely different...

% Este predicado auxiliar verifica se NomePessoa eh a pessoa que fica na
% cabeceira 1 (a que fica perto da lareira).
cab1(NomePessoa,Mesa):- Mesa = [[_,_,_],[NomePessoa,_],[_,_,_]].

% Este predicado auxiliar verifica se NomePessoa eh a pessoa
% que fica na cabeceira 2.
cab2(NomePessoa,Mesa):- Mesa = [[_,_,_],[_,NomePessoa],[_,_,_]].

% Este predicado auxiliar verifica se NomePessoa1 fica numa das
% cabeceiras e NomePessoa2 fica a sua direita.
honra(NomePessoa1,NomePessoa2,Mesa):-
    Mesa = ([[_,_,NomePessoa2],[_,NomePessoa1],[_,_,_]]);
    Mesa = ([[_,_,_],[NomePessoa1,_],[NomePessoa2,_,_]]).

% Este predicado auxiliar verifica se NomePessoa1 e NomePessoa2 ficam
% lado a lado na mesa.
lado(NomePessoa1,NomePessoa2,Mesa):-
    Mesa = ([[NomePessoa1,NomePessoa2,_],[_,_],[_,_,_]]);
    Mesa = ([[NomePessoa2,NomePessoa1,_],[_,_],[_,_,_]]);
    Mesa = ([[_,NomePessoa1,NomePessoa2],[_,_],[_,_,_]]);
    Mesa = ([[_,NomePessoa2,NomePessoa1],[_,_],[_,_,_]]);
    Mesa = ([[_,_,_],[_,_],[NomePessoa1,NomePessoa2,_]]);
    Mesa = ([[_,_,_],[_,_],[NomePessoa2,NomePessoa1,_]]);
    Mesa = ([[_,_,_],[_,_],[_,NomePessoa1,NomePessoa2]]);
    Mesa = ([[_,_,_],[_,_],[_,NomePessoa2,NomePessoa1]]).

% Este predicado auxiliar verifica se NomePessoa1 e NomePessoa2 nao
% ficam lado a lado na mesa.
naoLado(NomePessoa1,NomePessoa2,Mesa):-
    not(lado(NomePessoa1,NomePessoa2,Mesa)).

% Este predicado auxiliar verifica se NomePessoa1 e NomePessoa2 ficam
% exatamente frente a frente na mesa.
frente(NomePessoa1,NomePessoa2,Mesa):-
    Mesa = ([[NomePessoa1,_,_],[_,_],[NomePessoa2,_,_]]);
    Mesa = ([[NomePessoa2,_,_],[_,_],[NomePessoa1,_,_]]);
    Mesa = ([[_,NomePessoa1,_],[_,_],[_,NomePessoa2,_]]);
    Mesa = ([[_,NomePessoa2,_],[_,_],[_,NomePessoa1,_]]);
    Mesa = ([[_,_,NomePessoa1],[_,_],[_,_,NomePessoa2]]);
    Mesa = ([[_,_,NomePessoa2],[_,_],[_,_,NomePessoa1]]).

% Este predicado auxiliar verifica se NomePessoa1 e NomePessoa2 nao
% ficam frente a frente na mesa.
naoFrente(NomePessoa1,NomePessoa2,Mesa):-
    not(frente(NomePessoa1,NomePessoa2,Mesa)).

% Este predicado auxiliar verifica se OcupacaoMesa cumpre a Restricao.
ocupacaoMesa_aux(ListaPessoas,Restricao,OcupacaoMesa):-
    (Restricao = cab1(NomePessoa), member(NomePessoa,ListaPessoas),
     cab1(NomePessoa,OcupacaoMesa));

    (Restricao = cab2(NomePessoa), member(NomePessoa,ListaPessoas),
     cab2(NomePessoa,OcupacaoMesa));

    (Restricao = honra(NomePessoa1,NomePessoa2), member(NomePessoa1,ListaPessoas),
     member(NomePessoa2,ListaPessoas), honra(NomePessoa1,NomePessoa2,OcupacaoMesa));

    (Restricao = lado(NomePessoa1,NomePessoa2), member(NomePessoa1,ListaPessoas),
     member(NomePessoa2,ListaPessoas), lado(NomePessoa1,NomePessoa2,OcupacaoMesa));

    (Restricao = naoLado(NomePessoa1,NomePessoa2), member(NomePessoa1,ListaPessoas),
     member(NomePessoa2,ListaPessoas), naoLado(NomePessoa1,NomePessoa2,OcupacaoMesa));

    (Restricao = frente(NomePessoa1,NomePessoa2), member(NomePessoa1,ListaPessoas),
     member(NomePessoa2,ListaPessoas), frente(NomePessoa1,NomePessoa2,OcupacaoMesa));

    (Restricao = naoFrente(NomePessoa1,NomePessoa2), member(NomePessoa1,ListaPessoas),
     member(NomePessoa2,ListaPessoas), naoFrente(NomePessoa1,NomePessoa2,OcupacaoMesa)).

% Este predicado auxiliar serve para garantir que na solucao de
% OcupacaoMesa nao ha nenhuma pessoa em dois ou mais lugares diferentes
% em simultaneo.
diferente(OcupacaoMesa):-
    OcupacaoMesa = [[X1,X2,X3],[X4,X5],[X6,X7,X8]],
    X1 \== X2, X1 \== X3, X1 \== X4, X1 \== X5, X1 \== X6, X1 \== X7, X1 \== X8,
    X2 \== X3, X2 \== X4, X2 \== X5, X2 \== X6, X2 \== X7, X2 \== X8,
    X3 \== X4, X3 \== X5, X3 \== X6, X3 \== X7, X3 \== X8,
    X4 \== X5, X4 \== X6, X4 \== X7, X4 \== X8,
    X5 \== X6, X5 \== X7, X5 \== X8, X6 \== X7, X6 \== X8, X7 \== X8.

% Este predicado encontra e devolve uma solucao para dispor as pessoas
% de ListaPessoas ah mesa tendo em conta as restricoes de
% ListaRestricoes.
ocupacaoMesa(ListaPessoas,[Restricao|OutrasRestricoes],OcupacaoMesa):-
    OcupacaoMesa = [[X1,X2,X3],[X4,X5],[X6,X7,X8]],
    member(X1,ListaPessoas), member(X2,ListaPessoas), member(X3,ListaPessoas),
    member(X4,ListaPessoas), member(X5,ListaPessoas), member(X6,ListaPessoas),
    member(X7,ListaPessoas), member(X8,ListaPessoas),
    diferente(OcupacaoMesa),
    ocupacaoMesa_aux(ListaPessoas,Restricao,OcupacaoMesa),
    ocupacaoMesa(ListaPessoas,OutrasRestricoes,OcupacaoMesa).
ocupacaoMesa(_,[],_).
