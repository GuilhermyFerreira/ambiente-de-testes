#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define N 500       // número de linhas de A
#define M 500       // número de colunas de A e linhas de B
#define P 500       // número de colunas de B
#define NUM_THREADS 4  // quantidade de threads

int A[N][M], B[M][P], C[N][P];

// Estrutura para passar intervalo de linhas a cada thread
typedef struct {
    int start_row;
    int end_row;
} ThreadData;

// Função executada por cada thread
void* multiplicar(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int i, j, k;

    for (i = data->start_row; i < data->end_row; i++) {
        for (j = 0; j < P; j++) {
            int soma = 0;
            for (k = 0; k < M; k++) {
                soma += A[i][k] * B[k][j];
            }
            C[i][j] = soma;
        }
    }

    return NULL;
}

int main() {
    srand(time(NULL));

    // Inicializa matrizes A e B com valores aleatórios
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            A[i][j] = rand() % 10;
        }
    }
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < P; j++) {
            B[i][j] = rand() % 10;
        }
    }

    pthread_t threads[NUM_THREADS];
    ThreadData tdata[NUM_THREADS];

    int linhas_por_thread = N / NUM_THREADS;

    // Criação das threads
    for (int t = 0; t < NUM_THREADS; t++) {
        tdata[t].start_row = t * linhas_por_thread;
        tdata[t].end_row = (t == NUM_THREADS - 1) ? N : (t + 1) * linhas_por_thread;

        if (pthread_create(&threads[t], NULL, multiplicar, &tdata[t]) != 0) {
            perror("Erro ao criar thread");
            exit(1);
        }
    }

    // Espera todas as threads terminarem
    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    // Exibe os 10 primeiros elementos da matriz C
    printf("Primeiros 10 elementos da matriz resultante C:\n");
    int count = 0;
    for (int i = 0; i < N && count < 10; i++) {
        for (int j = 0; j < P && count < 10; j++) {
            printf("%d ", C[i][j]);
            count++;
        }
    }
    printf("\n");

    return 0;
}
