#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>

void soma_paralela(int comeco, int fim, int *soma, int *vetor, int passo, int id) {
    for (int i = comeco; i < fim; i += passo) {
        int limite = (i + passo < fim) ? i + passo : fim;
        for (int j = i; j < limite; j++) {
            *soma += vetor[j];
        }
        printf("[Filho %d | PID=%d] Somou elementos %d até %d (soma parcial = %d)\n",
               id, getpid(), i, limite - 1, *soma);
        sleep(1); // simula "andar no labirinto"
    }
    exit(0);
}

int main() {
    int shmid = shmget(IPC_PRIVATE, 2 * sizeof(int), IPC_CREAT | 0666);
    int *soma = (int *)shmat(shmid, NULL, 0);
    soma[0] = 0;
    soma[1] = 0;

    int tam = 10000;
    int *vetor = (int *)malloc(tam * sizeof(int));

    srand(time(NULL));
    for (int i = 0; i < tam; i++) {
        vetor[i] = rand() % 1000;
    }

    int meio = tam / 2;
    pid_t pid1 = fork();

    if (pid1 == 0) { 
        soma_paralela(0, meio, &soma[0], vetor, 1000, 1);
    } else { 
        pid_t pid2 = fork();
        if (pid2 == 0) { 
            soma_paralela(meio, tam, &soma[1], vetor, 1000, 2);
        } else {
            // Pai controla os filhos
            sleep(3);
            printf("\n--- Pausando processo %d ---\n", pid1);
            kill(pid1, SIGSTOP);

            sleep(3);
            printf("\n--- Retomando processo %d ---\n", pid1);
            kill(pid1, SIGCONT);

            sleep(3);
            printf("\n--- Matando processo %d ---\n", pid2);
            kill(pid2, SIGKILL);

            // Espera todos os processos terminarem
            wait(NULL);
            wait(NULL);

            int soma_total = soma[0] + soma[1];

            printf("\nSoma da primeira metade: %d\n", soma[0]);
            printf("Soma da segunda metade (pode estar incompleta se foi morto): %d\n", soma[1]);
            printf("Soma total: %d\n", soma_total);
            printf("Média (aprox): %.2f\n", (float)soma_total / tam);

            // Libera memória
            shmdt(soma);
            shmctl(shmid, IPC_RMID, NULL);
            free(vetor);
        }
    }

    return 0;
}