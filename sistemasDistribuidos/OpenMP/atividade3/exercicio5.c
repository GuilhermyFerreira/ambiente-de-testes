#include <stdio.h>
#include <omp.h>

int main (int argc, char *argv[]) {
    int max = 0;
    if (argc > 1) sscanf (argv[1], "%d", &max);
    if (max <= 0) return 1;

    int ts = omp_get_max_threads();
    int sums[ts];
    
    #pragma omp parallel
    {
        int t = omp_get_thread_num();
        int N_base = max / ts;
        int r = max % ts;

        int N_prev = t * N_base + (t < r ? t : r);
        int N_iter = N_base + (t < r ? 1 : 0);

        int t_lo = N_prev + 1; // Linha 12 modificada
        int hi = t_lo + N_iter - 1; // Linha 13 modificada

        sums[t] = 0;
        
        for (int i = t_lo; i <= hi; i++) {
            sums[t] = sums[t] + i;
        }
    }
    
    int sum = 0;
    for (int t = 0; t < ts; t++) sum = sum + sums[t];
    
    printf ("%d\n", sum);
    return 0;
}