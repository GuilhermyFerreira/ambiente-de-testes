#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 2

void* tarefa(int *num){
    
    for(int i=0; i < 10; i++){
        if(num[i] % 2 == 0){
            printf ("%d \n", num);
        }
        else
            printf ("%d \n", num);
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];
    int *num,i;

    for(i=0; i<10; i++){
        num[i]= i+1;
    }

     for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        if (pthread_create(&threads[i], NULL, tarefa(*num), &ids[i]) != 0) {
            perror("Erro ao criar thread");
            exit(1);
        }
    }


    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}
