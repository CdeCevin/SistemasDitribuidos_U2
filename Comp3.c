#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <math.h>

#define DIM 20
#define K 3
#define T 8

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

    scanf("%d", &SIZE);

    clock_t inicio = clock();
    omp_set_dynamic(0);
    omp_set_num_threads(T);

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

        #pragma omp parallel
        {
            int id = omp_get_thread_num();
            int num_threads = omp_get_num_threads();
            int elementos_por_hilo = SIZE / num_threads;
            int inicio = elementos_por_hilo * id;
            int fin = (id == num_threads - 1) ? SIZE : inicio + elementos_por_hilo;

            int cambio_local = 0;

            // Fase 1: Asignar elementos a centroides
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

            // Sincronizar antes de modificar centroides
            #pragma omp barrier

            // Fase 2: Recalcular centroides
            for (int c = 1; c <= K; c++) {
                static float suma_global[K][DIM];
                static int count_global[K];

                // Solo un hilo inicializa los arrays estáticos compartidos
                #pragma omp single
                {
                    for (int c2 = 0; c2 < K; c2++) {
                        for (int d = 0; d < DIM; d++)
                            suma_global[c2][d] = 0;
                        count_global[c2] = 0;
                    }
                }

                #pragma omp barrier

                float suma_local[K][DIM] = {0};
                int count_local[K] = {0};

                for (int i = inicio; i < fin; i++) {
                    int c = asignaciones[i][0] - 1;
                    for (int j = 0; j < DIM; j++) {
                        suma_local[c][j] += BD[i][j]; // Acumulación sin atomic
                    }
                    count_local[c]++;
                }

                #pragma omp critical
                {
                    for (int c = 0; c < K; c++) {
                        for (int j = 0; j < DIM; j++) {
                            suma_global[c][j] += suma_local[c][j]; // Un solo acceso seguro
                        }
                        count_global[c] += count_local[c];
                    }
                }

                #pragma omp barrier

                #pragma omp single
                {
                    if (count_global[c - 1] > 0) {
                        for (int j = 0; j < DIM; j++) {
                            centroide[c - 1][j] = suma_global[c - 1][j] / count_global[c - 1];
                        }
                    }
                }

                #pragma omp barrier
            }
        }

    } while (cambio);

    clock_t fin = clock();
    
    for (i = 0; i < SIZE; i++)
        printf("%d\n", asignaciones[i][0]);
    
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
