## OpenMP ##

-Para usar o openMP é necssario incluir **#include <omp.h>**
-Para compilar **gcc -fopenmp "nomeDoArquivo.c"** , porém isso criará um arquivo que por padrão em alguns sistemas é "a.exe || a.out"
-Para compilar da forma recomendada é **gcc -fopenmp "nomeDoArquivo.c" -o "nomeDoArquivo_omp"**
-Para executar **"./nomeDoArquivo_omp"**
