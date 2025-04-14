#include <stdio.h> 
#include <stdlib.h>
#define DIM 20
#define T 8
#define K 3


int main() {
    int i,j,SIZE;
    float **BD;
    scanf("%d",&SIZE);

<<<<<<< HEAD
    BD = (float **)malloc(sizeof(float *)*SIZE); 
=======
    BD = (float **)malloc(sizeof(float *)*SIZE); //aloja memoria para SIZE filas en la matriz BD
>>>>>>> 09c6a839906ff57e1fcbda55f04a4ddae84f8d55
    for(i=0;i<SIZE;i++)
        BD[i] = (float *)malloc(sizeof(float)*DIM); //aloja memoria para cada fila de la matriz BD

    for(i=0;i<SIZE;i++)
        for(j=0;j<DIM;j++)
            scanf("%f",&(BD[i][j]));
    
    printf("%d\n",SIZE);
    for(i=0;i<SIZE;i++)
    {
        for(j=0;j<DIM;j++)
            printf("%.2f ",BD[i][j]);
        printf("\n");
    }
    printf("Hola tonota");
    int a = 15;
    int b = 2;
    int c = a + b;
    printf("The sum of %d and %d is %d\n", a, b, c);
    
    return 0;
}
