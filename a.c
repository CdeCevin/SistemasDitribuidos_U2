#include <stdio.h> 
#include <stdlib.h>
#define DIM 20
#define T 8
#define K 3


int main() {
    int i,j,SIZE;
    float **BD;
    scanf("%d",&SIZE);

    BD = (float **)malloc(sizeof(float *)*SIZE);
    for(i=0;i<SIZE;i++)
        BD[i] = (float *)malloc(sizeof(float)*DIM);

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
