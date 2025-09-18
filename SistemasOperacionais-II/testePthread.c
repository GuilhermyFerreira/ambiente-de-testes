#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 2
#define ARRAY_SIZE 10

// Estrutura para passar o array e o ID da thread
typedef struct {
    int* array;
    int thread_id;
} ThreadData;

// A função da thread agora processa o array com base no ID
void* tarefa(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int thread_id = data->thread_id;
    int* num = data->array;
    
    printf("Thread %d: Iniciando o processamento...\n", thread_id);
    
    // A thread 0 (ID 0) processa números pares
    if (thread_id == 0) {
        for (int i = 0; i < ARRAY_SIZE; i++) {
            if (num[i] % 2 == 0) {
                printf("Thread %d: O número %d é par.\n", thread_id, num[i]);
            }
        }
    } 
    // A thread 1 (ID 1) processa números ímpares
    else if (thread_id == 1) {
        for (int i = 0; i < ARRAY_SIZE; i++) {
            if (num[i] % 2 != 0) {
                printf("Thread %d: O número %d é ímpar.\n", thread_id, num[i]);
            }
        }
    }
    
    printf("Thread %d: Processamento finalizado.\n", thread_id);
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];
    
    // Aloca memória para o array de números
    int* num = (int*)malloc(ARRAY_SIZE * sizeof(int));
    if (num == NULL) {
        perror("Erro na alocação de memória");
        return 1;
    }

    // Inicializa o array com valores de 1 a 10
    for (int i = 0; i < ARRAY_SIZE; i++) {
        num[i] = i + 1;
    }

    // Cria as threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].array = num;
        thread_data[i].thread_id = i;

        if (pthread_create(&threads[i], NULL, tarefa, (void*)&thread_data[i]) != 0) {
            perror("Erro ao criar thread");
            free(num);
            exit(1);
        }
    }

    // Espera as threads terminarem a execução
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Libera a memória alocada
    free(num);
    
    printf("Todas as threads foram finalizadas.\n");
    return 0;
}