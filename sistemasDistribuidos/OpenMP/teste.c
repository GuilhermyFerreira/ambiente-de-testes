#include <stdio.h>
#include <omp.h>

int main(){
    int i=0;
    //for(i=0;i<2;i++)
    #pragma omp parallel
    {
        #pragma omp sigle nowait
        {
            printf("somente uma thread rodando\n");
        }
        printf("olaaaaa \n");

        #pragma omp master
        {
            printf("Somente a thread mestre rodando\n");
        }
    }
    
    printf("Fora do fork\n");
}


