// Nombres Estudiantes: Kevin Olivares y Dominique Teillier
// Nombre Profesor: Ricardo Barrientos Rojel
// Fecha: 2025-04-28
// Ramo: Sistemas Distribuidos secc-2

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <math.h>

#define DIM 20
#define K 3
#define T 8

float calcular_distancia(float *fila, float *centroide);
void recalcular_centroides(float **BD, int SIZE, int **asignaciones, float **centroide);

int main() {
    int i, j, SIZE;
    float **BD;

    scanf("%d", &SIZE);

    omp_set_dynamic(0);
    omp_set_num_threads(T);

    BD = (float **)malloc(sizeof(float *) * SIZE);
    for (i = 0; i < SIZE; i++)
        BD[i] = (float *)malloc(sizeof(float) * DIM);

    for (i = 0; i < SIZE; i++)
        for (j = 0; j < DIM; j++)
            scanf("%f", &BD[i][j]);

    srand(time(NULL));

    double inicio = omp_get_wtime();

    float **centroide = (float **)malloc(sizeof(float *) * K);
    for (i = 0; i < K; i++)
        centroide[i] = (float *)malloc(sizeof(float) * DIM);

    int usados[SIZE];
    for (i = 0; i < SIZE; i++)
        usados[i] = 0;

    // Seleccionar K elementos aleatorios como centroides iniciales
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

        #pragma omp parallel
        {
            int id = omp_get_thread_num();
            int num_threads = omp_get_num_threads();
            int elementos_por_hilo = SIZE / num_threads;
            int inicio = elementos_por_hilo * id;
            int fin = (id == num_threads - 1) ? SIZE : inicio + elementos_por_hilo;

            int cambio_local = 0;

            for (int i = inicio; i < fin; i++) {
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
                    cambio_local = 1;
            }

            if (cambio_local) {
                #pragma omp atomic write
                cambio = 1;
            }
        }

        // Recalcular los centroides secuencialmente
        recalcular_centroides(BD, SIZE, asignaciones, centroide);

    } while (cambio);

    double fin = omp_get_wtime();

    for (i = 0; i < SIZE; i++)
        printf("%d\n", asignaciones[i][0]);

    printf("Tiempo de ejecucion: %.4f segundos\n", fin - inicio);

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

float calcular_distancia(float *fila, float *centroide) {
    float distancia = 0.0;
    for (int i = 0; i < DIM; i++) {
        float diff = fila[i] - centroide[i];
        distancia += diff * diff;
    }
    return sqrt(distancia);
}

void recalcular_centroides(float **BD, int SIZE, int **asignaciones, float **centroide) {
    float suma[K][DIM] = {0};
    int count[K] = {0};

    // Acumular suma y cantidad de elementos por cluster
    for (int i = 0; i < SIZE; i++) {
        int c = asignaciones[i][0] - 1;
        for (int j = 0; j < DIM; j++) {
            suma[c][j] += BD[i][j];
        }
        count[c]++;
    }

    // Recalcular centroides
    for (int c = 0; c < K; c++) {
        if (count[c] > 0) {
            for (int j = 0; j < DIM; j++) {
                centroide[c][j] = suma[c][j] / count[c];
            }
        }
    }
}
