## OpenMP ##

-Para usar o openMP é necssario incluir **#include <omp.h>**

-Para compilar **gcc -fopenmp "nomeDoArquivo.c"** , porém isso criará um arquivo que por padrão em alguns sistemas é "a.exe || a.out"

-Para compilar da forma recomendada é **gcc -fopenmp "nomeDoArquivo.c" -o "nomeDoArquivo_omp"**

-Para executar com um numero especifico de threads: **OMP_NUM_THREADS=4** se for para linux
e para windows: **$env:OMP_NUM_THREADS=4**

-Para executar **"./nomeDoArquivo_omp"**

# Introdução - OpenMP

## Processo, Threads e Memória

- Em um processo, todos os threads compartilham o mesmo **espaço de endereçamento.**
- **Stack**
  - Pertence ao thread.
  - **Privado** por natureza.
  - Assim, variáveis no Stack tendem a ser privadas.
  - O Stack dura só enquanto a função está ativa, ao sair, aquelas variáveis "somem".
- **Heap**
  - Pertence ao processo
  - **Compartilhado** por todas os threads.
  - Variáveis no heap tendem a ser compartilhadas.
  - É nessa área que pode ocorrer condição de corrida.


## Modelo Fork-Join (Como o paralelismo "Abre e Fecha")

### Ideia Central

1. O programa começa sequencialmente com **um thread** (thread main).
   1. Ao entrar numa região paralela, **o thread** faz fork de um **time de threads.**
      - Todos os threads do time executam o mesmo bloco de código (cada uma com sua stack/pilha). 
      - Time de threds: conjunto criado pela diretiva `parallel. Uma delas `
   
    ```c
        #pragma omp parallel
        {
            // Região paralela!    
        }
    ```

   2. Ao final do bloco, existe um **join implícito**: os threads terminam e a execução volta a ser sequencial

Uma delas é a zero normalmente a master é a zero.

Todas as threads executam o mesmo código, mas pode acontecer, que seja desejado que somente o thread master execute (#pragma omp master). "omp sigle" somente uma única thread é executada.
