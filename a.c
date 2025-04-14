#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define DIM 20
#define T 8
#define K 3

// Función para calcular la distancia euclidiana entre dos vectores de tamaño DIM
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
        printf("Centroide %d tomado de fila %d: (", i + 1, idx);
        for (j = 0; j < DIM; j++) {
            printf("%.2f", centroide[i][j]);
            if (j < DIM - 1)
                printf(" ");
        }
        printf(")\n");
    }

    // Calcular distancias euclidianas
    printf("\nDistancias Euclidianas:\n");
    for (i = 0; i < SIZE; i++) {
        printf("Fila %d:\n", i);
        for (int c = 0; c < K; c++) {
            float dist = calcular_distancia(BD[i], centroide[c]);
            printf("  Distancia al centroide %d: %.2f\n", c + 1, dist);
        }
    }

    // Arreglo para guardar el centroide más cercano a cada fila
    int asignaciones[SIZE][2];

    printf("\nAsignación de filas al centroide más cercano:\n");

    for (i = 0; i < SIZE; i++) {
        float min_dist = calcular_distancia(BD[i], centroide[0]);
        int centroide_mas_cercano = 0;

        for (int c = 1; c < K; c++) {
            float dist = calcular_distancia(BD[i], centroide[c]);
            if (dist < min_dist) {
                min_dist = dist;
                centroide_mas_cercano = c;
            }
        }

        asignaciones[i][0] = centroide_mas_cercano + 1; // +1 por legibilidad
        asignaciones[i][1] = i;

        printf("(Fila: %d,Centroide: %d)\n", asignaciones[i][1], asignaciones[i][0]);
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
