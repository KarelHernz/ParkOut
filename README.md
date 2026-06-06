[relatorio_bfs_vs_astar.md](https://github.com/user-attachments/files/28664740/relatorio_bfs_vs_astar.md)
## **BFS VS A*** 

_Análise Comparativa de Algoritmos de Busca e suas Complexidades_ 



**----- 
Documento: Relatório Técnico de Engenharia / Contexto: Estruturas de Dados e Algoritmos de<br>Ciência da Computação Inteligência Artificial<br>Variáveis: b  = fator de ramificação;  d  = Foco: Análise Assintótica de Tempo e<br>profundidade do nó objetivo Espaço (Notação Big-O)<br>**----- End of picture text -----**<br>


## **1. Introdução** 

A resolução de problemas baseados em grafos e árvores de estados constitui um dos pilares fundamentais da Ciência da Computação e da Inteligência Artificial. A escolha do algoritmo ideal para a travessia ou localização  do  caminho  mínimo  ( _shortest  path_ )  depende  diretamente  do  conhecimento  prévio  sobre  o ambiente. 

Este relatório analisa e confronta dois dos algoritmos mais célebres nesta área: o **BFS (Breadth-First Search / Busca em Largura)** , um algoritmo de estratégia cega ou não informada, e o **A* (A-Estrela)** , uma abordagem informada fundamentada em funções heurísticas. O foco principal reside na dissecação de suas complexidades de tempo e espaço sob a notação assintótica. 

## **2. BFS: Busca em Largura (Abordagem Cega)** 

O BFS expande a fronteira uniformemente através dos níveis do grafo. Ele explora todos os nós vizinhos à profundidade atual antes de avançar para os nós do nível seguinte, utilizando internamente uma estrutura de dados do tipo Fila (FIFO). 

## **2.1 Complexidade de Tempo** 

No pior caso, o BFS precisa de gerar e examinar todos os nós até alcançar a profundidade _**d**_ da solução ideal. Considerando um fator de ramificação médio _**b**_ (onde cada nó expande para _**b**_ novos nós), o total de nós processados segue uma progressão geométrica: 



Dominado pelo termo de maior ordem, a complexidade temporal assintótica do BFS é definida como: 

_**O(b[d] )**_ 

Relatório Técnico: BFS vs A* 


## **2.2 Complexidade de Espaço** 

A memória é o grande limitador do BFS. Para assegurar que a exploração ocorre estritamente por níveis e para reconstruir o caminho no final, o algoritmo armazena todos os nós gerados na memória. No nível _**d**_ , a cauda da fila conterá aproximadamente _**b[d]**_ nós. Assim, a complexidade espacial partilha o comportamento exponencial: 

## _**O(b[d] )**_ 

## **3. A*: Busca Informada (Heurística)** 

O algoritmo A* otimiza a exploração combinando o custo acumulado real para atingir um nó com uma estimativa futura do custo restante até ao objetivo. Ele avalia cada nó _**n**_ através da função: 

## _**f(n) = g(n) + h(n)**_ 

Onde _**g(n)**_ representa o custo real do caminho inicial até ao nó _**n**_ , e _**h(n)**_ denota a função heurística (estimativa do custo de _**n**_ até ao destino). 

## **3.1 Complexidade de Tempo** 

A complexidade temporal do A* está intrinsecamente ligada à precisão da heurística adotada: 

- **Pior Caso:** Se a heurística for nula ( _**h(n) = 0**_ ) ou não informativa, o A* comporta-se exatamente como o Algoritmo de Dijkstra. No pior cenário, onde o espaço de busca inteiro precisa de ser varrido, a complexidade converge para _**O(b[d] )**_ . 

- **Cenário Realista (Heurística Admissível/Consistente):** Uma boa heurística restringe o número de nós expandidos. Se o erro da heurística for controlado por _**|h(n) - h*(n)| ≤ O(log h*(n))**_ , a complexidade pode decair para uma escala polinomial ou até linear em casos ideais. 

## **3.2 Complexidade de Espaço** 

Para garantir a otimalidade e evitar ciclos, o A* mantém registo de todos os nós gerados em duas listas: a _Open List_ (nós descobertos a aguardar avaliação) e a _Closed List_ (nós já expandidos). Consequentemente, mesmo  direcionando  a  busca  de  forma  inteligente,  o  pior  caso  exige  a  retenção  de  uma  quantidade exponencial de estados: 

## _**O(b[d] )**_ 

Relatório Técnico: BFS vs A* 



## **4. Confronto Direto e Diferenças de Complexidade** 

|**Métrica de Análise**|**BFS (Busca em Largura)**|**A* (A-Estrela)**|
|---|---|---|
|**Natureza da Busca**|Cega / Custo Uniforme por níveis.|Informada / Guiada por Heurística.|
|**Complexidade de**<br>**Tempo**|**_O(bd)_**(Sempre exponencial).|**_O(bd)_**no pior caso; reduz drasticamente com<br>heurísticas precisas.|
|**Complexidade de**<br>**Espaço**|**_O(bd)_**(Armazena toda a fronteira<br>ativa).|**_O(bd)_**(Retém todos os nós avaliados e na<br>fronteira).|



## **Conclusão e Diagnóstico de Engenharia** 

Embora a análise matemática formal em "pior caso" classifique ambos os algoritmos com a mesma complexidade assintótica ( _**O(b[d] )**_ ), o comportamento em ambientes reais difere drasticamente. 

O BFS funciona como uma expansão esférica uniforme, sendo ineficiente para espaços de estados profundos ou complexos devido ao consumo intolerável de memória RAM. O A*, por outro lado, canaliza os recursos de processamento diretamente em direção ao alvo graças ao vetor heurístico, reduzindo drasticamente a constante oculta da notação Big-O. O gargalo prático do A* permanece o espaço, o que levou ao desenvolvimento de variantes de memória limitada como o IDA* ou SMA*. 

Relatório Técnico: BFS vs A* 


