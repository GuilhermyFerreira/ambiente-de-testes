#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 16

void prefix_sum(long *vec, long *result) {
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        result[i] = vec[i];
    }

    for (int k = 0; k < 4; k++) {
        int dist = 1 << k;

        #pragma omp parallel for
        for (int i = dist; i < N; i++) {
            result[i] += result[i - dist];
        }
    }
}

void print_vector(const char *name, long *vec) {
    printf("%s: [", name);
    for (int i = 0; i < N; i++) {
        printf("%ld%s", vec[i], (i < N - 1) ? ", " : "");
    }
    printf("]\n");
}

int main() {
    long vec1[N];
    for (int i = 0; i < N; i++) {
        vec1[i] = 1;
    }
    long result1[N];

    long vec2[N];
    for (int i = 0; i < N; i++) {
        vec2[i] = i + 1;
    }
    long result2[N];

    printf("Soma de Prefixos Inclusiva (N=16)\n");

    prefix_sum(vec1, result1);
    print_vector("Vetor Original 1", vec1);
    print_vector("Soma de Prefixos 1", result1);

    printf("\n");

    prefix_sum(vec2, result2);
    print_vector("Vetor Original 2", vec2);
    print_vector("Soma de Prefixos 2", result2);

    return 0;
}