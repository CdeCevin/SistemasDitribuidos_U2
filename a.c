#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define DIM 20
#define K 3

// Función para calcular la distancia euclidiana entre dos vectores
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

    // Imprimir matriz con índices
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

    // Seleccionar K filas aleatorias únicas como centroides
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

        printf("Centroide %d tomado de fila %d: (", i + 1, idx);
        for (j = 0; j < DIM; j++) {
            printf("%.2f", centroide[i][j]);
            if (j < DIM - 1) printf(" ");
        }
        printf(")\n");
    }

    // Asignar cada fila a su centroide más cercano
    int **asignaciones = (int **)malloc(sizeof(int *) * SIZE);
    for (i = 0; i < SIZE; i++)
        asignaciones[i] = (int *)malloc(sizeof(int) * 2); // [0]=centroide, [1]=fila

    int *asignaciones_previas = (int *)malloc(sizeof(int) * SIZE);
    for (i = 0; i < SIZE; i++)
        asignaciones_previas[i] = -1; // valor imposible al inicio

    int cambio, iter = 1;

    do {
        cambio = 0;
        printf("\nIteracion %d\n", iter++);

        // Copiar asignaciones actuales a previas
        for (i = 0; i < SIZE; i++)
            asignaciones_previas[i] = asignaciones[i][0];

        // Asignar filas a centroides
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

            printf("Fila %d asignada al centroide %d (distancia: %.2f)\n", i, minIdx, minDist);
        }

        if (cambio)
            printf("¡Hubo cambios en esta iteracion!\n");

        // Recalcular centroides
        printf("Actualizando centroides...\n");
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

                printf("Centroide %d actualizado: (", c);
                for (j = 0; j < DIM; j++) {
                    printf("%.2f", centroide[c - 1][j]);
                    if (j < DIM - 1) printf(" ");
                }
                printf(")\n");
            }
        }

    } while (cambio);

    printf("\nListoke! No hubo mas cambios en asignaciones.\n");

    // Liberar memoria
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
