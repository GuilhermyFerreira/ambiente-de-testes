#include <stdio.h>
#include <omp.h>

int main(void)
{
    const long N = 1000000;
    static double A[1000000];
    #pragma omp parallel for
    for (long i = 0; i < N; i++)
    {
        // cada iteração escreve em A[i]
        A[i] = 1.0 / (i + 1);
    } // barreira implícita aqui
    printf("A[0]=%.3f  A[%ld]=%.6f\n", A[0], N - 1, A[N - 1]);
    return 0;
}