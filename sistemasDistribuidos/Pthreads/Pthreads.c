#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define LINES 5
#define NUM_THREADS 2

int matriz1[LINES][LINES],matriz2[LINES][LINES],matriz3[LINES][LINES];

typedef struct {
    int inicio;
    int fim;
} ThreadData;

void* multiplicar(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    for (int i = data->inicio; i < data->fim; i++) {
        for (int j = 0; j < LINES; j++) {
            matriz3[i][j] = 0;
            for (int k = 0; k < LINES; k++) {
                matriz3[i][j] += matriz1[i][k] * matriz2[k][j];
            }
        }
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    ThreadData tdata[NUM_THREADS];

    srand(time(NULL));
    for (int i = 0; i < LINES; i++) {
        for(int j=0; j<LINES; j++){
            matriz1[i][j] = rand() % 10;
            matriz2[i][j] = rand() % 10;
        }
    }

    int linhas_por_thread = LINES / NUM_THREADS;
    for (int t = 0; t < NUM_THREADS; t++) {
        tdata[t].inicio = t * linhas_por_thread;
        tdata[t].fim = (t == NUM_THREADS - 1) ? LINES : (t + 1) * linhas_por_thread;
        pthread_create(&threads[t], NULL, multiplicar, &tdata[t]);
    }

    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    for (int i = 0; i < LINES; i++) {
        for (int j = 0; j < LINES; j++) {
            printf("%d ", matriz3[i][j]);
        }
        printf("\n");
    }

    return 0;
}
