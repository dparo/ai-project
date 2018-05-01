% A SAT Solver Implementation
% Davide Paro;
% May 2018


Parsing
=======
1. Il primo passo consiste nella tokenizzazione della formula
   ogni singolo carattere viene raggrupato in "tokens" secondo
   le regole sintattiche definite dal linguaggio. Il linguaggio
   mette a disposizione la possibilita' di definire commenti.
2. Una volta generati i "tokens", si procede alla conversione
   della formula dalla forma __infissa__ (piu' comoda agli umani)
   in una forma __prefissa__ (LISP Notation).

          (A & B) | C     -->    (| (& A B) C)
  
3. Si fa uso dell'algoritmo Shunting-Yard fortemente customizzato con
   estensioni per risolvere il problema in questione. In base
   a regole di precedenza degli operatori e alle regole sintattiche
   defininte per il linguaggio verra' generata la notazione prefissa
   appropriata
   

AST Generation
==============
1. Una volta "parsata" la formula in forma prefissa si
   genera l'albero di sintassi <**AST**>. L'albero
   di sintassi verra' implementato semplicemente con
   un semplice STACK.
2. Segue un analisi semantica e di validita' della formula
   presente nell'AST. Se erronea, viene rifiutata.

      A & | B   e' una formula mal formata -> errore

{{ PICTURES }}

Bruteforce solver
=================
> Genera una truth table

1. In base al numero di letterali trovati all'interno della
   formula si procede alla generazione di 2^n possibili combinazioni
   (dove n e' il numero di letterali)
2. Grazie al fatto di aver rappresentato l'AST con un semplice stack
   ci permette di implementare un risolutore che sostanzialmente
   lavora come una **stack-virtual-machine**.
   Si procede dal fondo dello stack per ogni letterale/costante
   si "pusha" il valore in un altro stack **(Virtual Machine Stack)**
   e per ogni operatore si fa il "pop" del numero necessario di operandi
   e si "ripusha" il risultato.
   Alla fine della "evaluation" dell'intero AST in caso di formula ben formata
   sullo stack delle computazioni rimane solamente un elemento:
   il risultato della computazione dell'intera formula.

{{ PICTURES }}

{{ DEMO }}

Performance Analysis
=====================
* Un risolutore basato su bruteforce comincia a diventare impraticabile
  gia' con 16 letterali.
  
          Esempio: "a1 | a2 | ... | an"

  N: Numero letterali        Tempo di computazione
  -------------------        ---------------------
                   15                 6 secondi
                   16                12 secondi
                   17                27 secondi
                   18                57 secondi
                   19               121 secondi

* Il tempo di computazione diventa piu' del doppio per ogni letterale
* Il tempo di computazione peggiora drasticamente se sono presenti
  sotto-formule coincidenti. Esse vengono valutate piu' volte dal
  risolutore

A Better Approach
=================
* Se ci interessa solamente dimostrare la soddisfacibilita' di una
  formula logica si puo' utilizzare un algoritmo che fa uso di 
  backtracking.
* **SODDISFACIBILITA'**: Si vuole dimostrare se la formula risulta
  sempre vera indipendentemente dall'assegnamento dei valori
  di input.


DPLL
====
* Introdotto nel 1962 da Martin Davis, George Logemann, Donald W. Loveland
* Algoritmo classico, e usato come base per algoritmi migliori
  che si sono evoluti da DPLL. Esempio:

        Chaff, zChaff, GRASP
  
  Sono delle implemntazioni piu' performanti e raffinate di **DPLL**
  
  [__Wikipedia Link__](https://en.wikipedia.org/wiki/DPLL_algorithm)
  
  
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ruby
     Algorithm DPLL
       Input: A set of clauses K.
       Output: A Truth Value.
     ----------------------------------------------
     function DPLL(K) {
        if K is a consistent set of literals
            then return true;
        if K contains an empty clause
            then return false;
        for every unit clause {l} in K
           K ← unit-propagate(l, K);
        for every literal l that occurs pure in K
           K ← pure-literal-assign(l, K);
        l ← choose-literal(K);
        return DPLL(K and {l}) or DPLL(K and {not(l)});
     }
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

CNF Conversion
==============
1. Si trasformano tutti gli operatori come __AND, OR, NOT__
        
        a ^ b   = 
        a -> b  = 
        a <-> b = ~(a ^ b) = 

