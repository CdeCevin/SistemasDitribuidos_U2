#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define DIM 20
#define K 3

float calcular_distancia(float *fila, float *centroide) {
    float distancia = 0.0;
    for (int i = 0; i < DIM; i++) {
        float diff = fila[i] - centroide[i];
        distancia += diff * diff;
    }
    return sqrt(distancia);
}

int main() {
    int i, j, SIZE;
    float **BD;

    clock_t inicio = clock(); // Comenzar medición de tiempo

    scanf("%d", &SIZE);

    BD = (float **)malloc(sizeof(float *) * SIZE);
    for (i = 0; i < SIZE; i++)
        BD[i] = (float *)malloc(sizeof(float) * DIM);

    for (i = 0; i < SIZE; i++)
        for (j = 0; j < DIM; j++)
            scanf("%f", &BD[i][j]);

    srand(time(NULL));

    float **centroide = (float **)malloc(sizeof(float *) * K);
    for (i = 0; i < K; i++)
        centroide[i] = (float *)malloc(sizeof(float) * DIM);

    int usados[SIZE];
    for (i = 0; i < SIZE; i++)
        usados[i] = 0;

    for (i = 0; i < K; i++) {
        int idx;
        do {
            idx = rand() % SIZE;
        } while (usados[idx]);
        usados[idx] = 1;

        for (j = 0; j < DIM; j++)
            centroide[i][j] = BD[idx][j];
    }

    int **asignaciones = (int **)malloc(sizeof(int *) * SIZE);
    for (i = 0; i < SIZE; i++)
        asignaciones[i] = (int *)malloc(sizeof(int) * 2);

    int *asignaciones_previas = (int *)malloc(sizeof(int) * SIZE);
    for (i = 0; i < SIZE; i++)
        asignaciones_previas[i] = -1;

    int cambio;

    do {
        cambio = 0;

        for (i = 0; i < SIZE; i++)
            asignaciones_previas[i] = asignaciones[i][0];

        for (i = 0; i < SIZE; i++) {
            float minDist = calcular_distancia(BD[i], centroide[0]);
            int minIdx = 1;

            for (int c = 1; c < K; c++) {
                float dist = calcular_distancia(BD[i], centroide[c]);
                if (dist < minDist) {
                    minDist = dist;
                    minIdx = c + 1;
                }
            }

            asignaciones[i][0] = minIdx;
            asignaciones[i][1] = i;

            if (asignaciones[i][0] != asignaciones_previas[i])
                cambio = 1;
        }

        for (int c = 1; c <= K; c++) {
            float suma[DIM] = {0};
            int count = 0;

            for (i = 0; i < SIZE; i++) {
                if (asignaciones[i][0] == c) {
                    for (j = 0; j < DIM; j++) {
                        suma[j] += BD[asignaciones[i][1]][j];
                    }
                    count++;
                }
            }

            if (count > 0) {
                for (j = 0; j < DIM; j++) {
                    centroide[c - 1][j] = suma[j] / count;
                }
            }
        }

    } while (cambio);

    clock_t fin = clock(); // Fin de medición
    
    for (i = 0; i < SIZE; i++) {
        printf("%d\n", asignaciones[i][0]);
    }

    
    double tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC;
    printf("Tiempo de ejecucion: %.4f segundos\n", tiempo);

    for (i = 0; i < SIZE; i++) {
        free(BD[i]);
        free(asignaciones[i]);
    }
    free(BD);
    free(asignaciones);
    free(asignaciones_previas);

    for (i = 0; i < K; i++)
        free(centroide[i]);
    free(centroide);

    return 0;
}
