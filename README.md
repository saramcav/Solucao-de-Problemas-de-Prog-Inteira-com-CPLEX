# Solucao-de-Problemas-de-Prog.-Inteira-com-CPLEX
Trabalhos realizados para a disciplina de Programação Inteira cursada em 2024.1 na Universidade Federal Fluminense.

## Trabalho 1 - Problema de Localização de Facilidades Capacitado (PLFC)

Sejam $I$ o conjunto de facilidades disponíveis para construção e $J$ o conjunto de clientes que devem ser atendidos. A relação entre esses conjuntos define um grafo bipartido não completo, onde as arestas $(i,j) \in E$ indicam que a facilidade $i \in I$, se construída, pode atender o cliente $j \in J$. 

Para cada conexão $(i,j) \in E$, seja $g_{ij}$ o custo de atendimento da facilidade $i$ ao cliente $j$, e $c$ o custo fixo de construção de qualquer facilidade. Além disso, cada facilidade $i \in I$ possui uma capacidade limitada de atendimento, dada por $Q$, enquanto que cada atendimento realizado pela facilidade $i$ ao cliente $j$ consome $p_{ij}$ unidades dessa capacidade.

O Problema de Localização de Facilidades Capacitado (PLFC) consiste em decidir quais facilidades devem ser construídas e como os clientes devem ser atendidos, de modo que todos os clientes sejam cobertos e o custo total seja minimizado.

Para modelar esse problema, são definidas as seguintes variáveis binárias:

- $y_i$ para cada facilidade $i \in I$, onde $y_i = 1$ se a facilidade for construída e $y_i = 0$ caso contrário.
- $x_{ij}$ para cada $(i,j) \in E$, onde $x_{ij} = 1$ se o cliente $j$ for atendido pela facilidade $i$ e $x_{ij} = 0$ caso contrário.


### Modelo

O código implementa o seguinte modelo:

$$
\min \sum_{i \in I} cy_i
 + \sum_{(i,j) \in E} g_{ij}x_{ij} \quad (1)
 $$

$$
\sum_{i \in I}  x_{ij} = 1, \quad \forall j \in J \quad (2)
$$

$$
x_{ij} \leq y_i, \quad \forall (i,j) \in E \quad (3)
$$

$$
\sum_{(i,j) \in E} p_{ij}x_{ij} \leq Q y_i, \quad \forall i \in I \quad (4)
$$

$$
x_{ij} \in \{0,1\}, \quad \forall (i,j) \in E \quad (5)
$$

$$
y_i \in \{0,1\}, \quad \forall i \in I \quad (6)
$$

### Instâncias de teste

Instâncias `.txt` estão na pasta deste trabalho e possuem o seguinte formato de arquivo:

$ni$ $nj$ $c$ $Q$ $NL$

$facilidade(i)$ $cliente(j)$ $g_{ij}$ $p_{ij}$

onde:

- $ni$: número de facilidades
- $nj$: número de clientes
- $c$: custo de abrir uma facilidade
- $Q$: quantidade de recursos em cada facilidade
- $NL$: número de linhas de facilidades/clientes a seguir

Exemplo:

2 3 50 5 5

1 1 20 2

1 2 15 2

1 3 10 2

2 1 40 2

2 3 30 2

#### Valores ótimos das instâncias

- `TPI_F_0.txt`: 165
- `TPI_F_1.txt`: 2842.8
- `TPI_F_2.txt`: 3013.7

## Trabalho 2 - Coloração

Seja $G = (V,E)$ um grafo não direcionando onde $|V|=n$. A implementação encontra o número cromático dos grafos dados de entrada. No modelo, são utilizadas as seguintes variáveis binárias:
- $x_{ij}$, para $i \in V$ e $1 \leq j \leq n$, onde $x_{ij} = 1$ se o vértice $i$ recebe a cor $j$ e 0 caso contrário. 
- $w_j$, para $j=1 \cdots n$ , que indica se a cor $j$ foi usada em algum vértice.

### Modelo

O código implementa o seguinte modelo:

Função Objetivo:

$$
\min \sum_{j=1}^{n} w_j \quad (1)
$$

S.a.:

$$
\sum_{j=1}^{n} x_{ij} = 1, \quad \forall i \in V \quad (2)
$$

$$
x_{ij} + x_{kj} \leq w_j, \quad \forall (i, k) \in E, \quad 1 \leq j \leq n \quad (3)
$$

$$
w_j \geq w_{j+1}, \quad \forall 1 \leq j \leq n-1 \quad (4)
$$

$$
w_j \leq \sum_{i \in V} x_{ij}, \quad \forall 1 \leq j \leq n \quad (5)
$$

$$
x_{ij} \in \{0,1\}, \quad \forall i \in V, \quad 1 \leq j \leq n \quad (6)
$$

### Instâncias de teste

Instâncias `.txt` estão na pasta deste trabalho e possuem o seguinte formato de arquivo:

$p$ $edge$ $n$ $m$

$e$ $vertice(i)$ $vertice(j)$ 

onde:

- $n$: número de vértices
- $m$: número de arestas

Exemplo:

p edge 11 20

e 1 2

e 1 4

e 1 7

e 1 9

e 2 3

e 2 6

$\quad \vdots$

#### Valores ótimos das instâncias


- `TPI_BC_COL_0.txt`: 5
- `TPI_BC_COL_1.txt`: 6
- `TPI_BC_COL_2.txt`: 9


## Trabalho 3 - Coloração Branch-and-Cut

### Modelo

Utilizado o mesmo modelo do trabalho 2

### Corte de usuário implementado

$$
\sum_{i \in S} x_{ij} \leq w_j, \quad \forall \text{ clique } S \subseteq V, \quad \forall 1 \leq j \leq n
$$

Não foram gerados todos os cortes, somente aqueles violados pela solução fracionária atual. 

O método procurou heuristicamente uma clique maximal que esteja violada, ou seja, 

Dada uma solução fracionária $(x^*, w^*)$ o método procurou por uma cor $j$ e uma clique $S$ em que:

$$
\sum_{i \in S} x_{ij}^{*} > w_j^{*} + \varepsilon
$$

para inserir os respectivos cortes de usuário no modelo.

### Heurística Implementada

Foi implementada uma heurística gulosa para achar a clique ponderada máxima por meio da métrica de **peso de um vértice** $\times$ **grau desse vértice**.

### Entrada e Saída

O programa dá ao usuário a opção de usar cortes de usuário ou executar sem corta algum (sem nenhum corte automático do CPLEX).

Ele imprime como saída:

- o valor da solução ótima
- o tempo total
- o número de cortes gerados no total

### Instâncias de teste

Instâncias `.txt` estão na pasta deste trabalho e possuem o  mesmo formato de arquivo das utilizadas no Trabalho 2.