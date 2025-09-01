#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

#define N 1000
#define NUM_PROCESSES 8

int main(void)
{
    int shmid_A = shmget(IPC_PRIVATE, N * sizeof(int), IPC_CREAT | 0666);
    int shmid_B = shmget(IPC_PRIVATE, N * sizeof(int), IPC_CREAT | 0666);
    int shmid_C = shmget(IPC_PRIVATE, N * sizeof(int), IPC_CREAT | 0666);

    if (shmid_A < 0 || shmid_B < 0 || shmid_C < 0)
    {
        perror("shmget");
        exit(1);
    }

    int *A = (int *)shmat(shmid_A, NULL, 0);
    int *B = (int *)shmat(shmid_B, NULL, 0);
    int *C = (int *)shmat(shmid_C, NULL, 0);
    if (A == (void *)-1 || B == (void *)-1 || C == (void *)-1)
    {
        perror("shmat");
        exit(1);
    }

    for (int i = 0; i < N; i++)
    {
        A[i] = i;
        B[i] = 2 * i;
    }

    int tam_seg = (N + NUM_PROCESSES - 1) / NUM_PROCESSES;

    for (int p = 0; p < NUM_PROCESSES; p++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            int start = p * tam_seg;
            int end = start + tam_seg;
            if (end > N)
                end = N;

            for (int i = start; i < end; i++)
                C[i] = A[i] + B[i];

            _exit(0);
        }
    }

    for (int p = 0; p < NUM_PROCESSES; p++)
        wait(NULL);

    for (int i = 0; i < N; i++)
    {
        printf("%d%c", C[i], (i + 1) % 20 == 0 ? '\n' : ' ');
    }
    if (N % 20)
        puts("");

    shmdt(A);
    shmdt(B);
    shmdt(C);
    shmctl(shmid_A, IPC_RMID, NULL);
    shmctl(shmid_B, IPC_RMID, NULL);
    shmctl(shmid_C, IPC_RMID, NULL);
    return 0;
}
