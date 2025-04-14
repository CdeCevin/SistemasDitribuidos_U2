#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DIM 20
#define T 8
#define K 3

int main() {
    int i, j, SIZE;
    float **BD;

    // Leer tamaño
    scanf("%d", &SIZE);

    // Reservar memoria para la matriz BD (SIZE x DIM)
    BD = (float **)malloc(sizeof(float *) * SIZE);
    for (i = 0; i < SIZE; i++)
        BD[i] = (float *)malloc(sizeof(float) * DIM);

    // Leer datos
    for (i = 0; i < SIZE; i++)
        for (j = 0; j < DIM; j++)
            scanf("%f", &BD[i][j]);

    // Imprimir matriz con índices y paréntesis
    printf("%d\n", SIZE);
    for (i = 0; i < SIZE; i++) {
        printf("%d(", i);
        for (j = 0; j < DIM; j++) {
            printf("%.2f", BD[i][j]);
            if (j < DIM - 1)
                printf(" ");
        }
        printf(")\n");
    }

    // Inicializar semilla aleatoria
    srand(time(NULL));

    // Reservar memoria para los centroides (K x DIM)
    float **centroide = (float **)malloc(sizeof(float *) * K);
    for (i = 0; i < K; i++)
        centroide[i] = (float *)malloc(sizeof(float) * DIM);

    // Seleccionar K filas aleatorias únicas
    int usados[SIZE];
    for (i = 0; i < SIZE; i++)
        usados[i] = 0;

    for (i = 0; i < K; i++) {
        int idx;
        do {
            idx = rand() % SIZE;
        } while (usados[idx]);
        usados[idx] = 1;

        // Copiar fila como centroide
        for (j = 0; j < DIM; j++)
            centroide[i][j] = BD[idx][j];

        // Imprimir centroide
        printf("Centroide %d tomado de fila %d: (", i+1, idx);
        for (j = 0; j < DIM; j++) {
            printf("%.2f", centroide[i][j]);
            if (j < DIM - 1)
                printf(" ");
        }
        printf(")\n");
    }

    // Liberar memoria
    for (i = 0; i < SIZE; i++)
        free(BD[i]);
    free(BD);

    for (i = 0; i < K; i++)
        free(centroide[i]);
    free(centroide);

    return 0;
}
