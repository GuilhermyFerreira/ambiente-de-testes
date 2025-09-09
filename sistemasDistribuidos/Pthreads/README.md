## Deyvid Soares de Souza Ferreira
## Gabriel Moreira de Souza
## João Vitor Peixoto Martins
## Augusto de Paula e Campos
## Guilhermy Ferreira da Silva

1- Foi utilizado um total de 2 threads  
A divisão foi feita de forma que cada thread processasse uma parte das linhas da matriz de forma paralela. 

2- Foram utilizdas 3 matrizes estaticas: matriz1, matriz2 e matriz3, e foi criada uma struct para armazenar o intervalo de linhas que cada thread deve processar.

3- Os resultados da multiplicação são armazenados diretamente na matriz3, que está declarada como variável global.

4- A maior dificuldade foi na falta de conhecimento da biblioteca Pthreads e na forma de implmentar uma matriz de forma paralela