#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define PROCESSOS 4

void soma_paralela(int comeco, int fim, int *soma, int *vetor) {
    for (int i = comeco; i < fim; i++) {
        *soma += vetor[i];
    }
}

int main() {
    int shmid = shmget(IPC_PRIVATE, PROCESSOS * sizeof(int), IPC_CREAT | 0666);
    int *soma = (int *)shmat(shmid, NULL, 0);

    int tam = 20; 
    int soma_total=0;
    int meio=tam/PROCESSOS;
    int *vetor = (int *)malloc(tam * sizeof(int));

    srand(time(NULL));
    for (int i = 0; i < tam; i++) {
        vetor[i] = rand() % 10;
    }

    
    for (int i = 0; i < PROCESSOS; i++){
        pid_t pid0 = fork();
        if (pid0 == 0) { 
            soma_paralela(i*meio, ((i+1)*meio), &soma[i], vetor);
            exit(0);
        }
    }   
    

    for(int i = 0; i < PROCESSOS; i++){
        wait(NULL);
        soma_total+=soma[i];        
    }

    printf("Soma da primeira metade: %d\n", soma[0]);
    printf("Soma da segunda metade: %d\n", soma[1]);
    printf("Soma da terceira metade: %d\n", soma[2]);
    printf("Soma da quarta metade: %d\n", soma[3]);
    printf("Soma total: %d\n", soma_total);
    printf("Média: %.2f\n", (float)soma_total / tam);
    shmdt(soma);
    shmctl(shmid, IPC_RMID, NULL);
    free(vetor);

    return 0;
}