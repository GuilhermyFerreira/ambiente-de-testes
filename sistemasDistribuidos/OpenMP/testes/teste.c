#include <stdio.h>
#include <omp.h>

int main(){
    int vector[10];;
    for(int i=0;i<10;i++){
        vector[i] = i+1;
    }
    printf("Dentro do fork\n");
    #pragma omp parallel
    {   
        #pragma omp master
        {
            printf("Thread %d: ", omp_get_thread_num());
            for(int i=0;i<10;i++){
            printf("%d \n", vector[i]);
            }
        }
        
        
    }
    printf("Fora do fork\n");
}


