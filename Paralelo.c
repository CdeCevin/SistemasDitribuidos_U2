// Nombres Estudiantes: Kevin Olivares y Dominique Teillier
// Nombre Profesor: Ricardo Barrientos Rojel
// Fecha: 2025-04-28
// Ramo: Sistemas Distribuidos secc-2

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <math.h>

// Definicion de constantes
#define DIM 20
#define K 3
#define T 8

// Declaracion de funciones
float calcular_distancia(float *fila, float *centroide);

int main() {
    int i, j, SIZE;
    float **BD;

    scanf("%d", &SIZE);

    omp_set_dynamic(0);
    omp_set_num_threads(T);

    // Reserva de memoria dinamica para la base de datos y lectura
    BD = (float **)malloc(sizeof(float *) * SIZE);
    for (i = 0; i < SIZE; i++)
        BD[i] = (float *)malloc(sizeof(float) * DIM);

    for (i = 0; i < SIZE; i++)
        for (j = 0; j < DIM; j++)
            scanf("%f", &BD[i][j]);

    srand(time(NULL));
    double inicioT = omp_get_wtime();

    // Reserva de memoria dinamica para K filas de los centroides con DIM elementos
    float **centroide = (float **)malloc(sizeof(float *) * K);
    for (i = 0; i < K; i++)
        centroide[i] = (float *)malloc(sizeof(float) * DIM);

    // Arreglo auxiliar para reconocer los centroides ya escogidos, se inicializan todos en 0
    int usados[SIZE];
    for (i = 0; i < SIZE; i++)
        usados[i] = 0;

    // Seleccionar K elementos aleatorios como centroides iniciales entre 0 y SIZE-1
    // El ciclo se repite hasta encontrar un indice que no haya sido utilizado
    for (i = 0; i < K; i++) {
        int idx;
        do {
            idx = rand() % SIZE;
        } while (usados[idx]);
        usados[idx] = 1;

        // Copiar el elemento como centroide
        for (j = 0; j < DIM; j++)
            centroide[i][j] = BD[idx][j];
    }

    // Se reserva memoria dinamica para la matriz de asignaciones con la forma [indice cluster, indice vector]
    int **asignaciones = (int **)malloc(sizeof(int *) * SIZE);
    for (i = 0; i < SIZE; i++)
        asignaciones[i] = (int *)malloc(sizeof(int) * 2);
    // Se reserva memoria dinamica para la matriz de asignaciones previas que permite detectar cambios entre iteraciones
    int *asignaciones_previas = (int *)malloc(sizeof(int) * SIZE);
    for (i = 0; i < SIZE; i++)
        asignaciones_previas[i] = -1;


    int cambio;

    // Algoritmo principal K-means
    // Se repite mientras existan cambios en las asignaciones vector-centroide
    do {
        cambio = 0;

        // Copia las asignaciones de la iteracion a las asignaciones previas antes de hacer el siguiente calculo
        for (i = 0; i < SIZE; i++)
            asignaciones_previas[i] = asignaciones[i][0];

        // Para realizar el trabajo en paralelo este se divide entre los hilos
        #pragma omp parallel
        {
            int id = omp_get_thread_num();
            int num_threads = omp_get_num_threads();
            int elementos_por_hilo = SIZE / num_threads;
            int inicio = elementos_por_hilo * id;
            int fin;
            if(id == num_threads-1){
                fin = SIZE;
            }
            else{
                fin = inicio + elementos_por_hilo;
            }
            int cambio_local = 0;

            // Fase 1: Asignar elementos de la base de datos a los centroides segun el calculo de su distancia euclidiana
            // Se calcula la distancia respecto al primer centroide como base, luego esta se compara con las distancias calculadas
            // para el resto de centroides y se asocia el indice del vector al indice del centroide con la distancia mas baja 
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

                // Si existe un cambio respecto a la iteracion anterior se señala para continuar con el algoritmo
                if (asignaciones[i][0] != asignaciones_previas[i])
                    cambio_local = 1;
            }

            if (cambio_local) {
                #pragma omp atomic write
                cambio = 1;
            }

            // Sincronizar antes de modificar centroides
            #pragma omp barrier

            // Fase 2: Por cada centroide recalcular su valor a traves de los vectores asignados a este
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

                // Se divide el trabajo entre los hilos, cada hilo acumula (dentro de su variable local) la suma 
                // de los vectores asignados a cada centroide y se registra la cantidad de vectores asignados a este
                for (int i = inicio; i < fin; i++) {
                    int c = asignaciones[i][0] - 1;
                    for (int j = 0; j < DIM; j++) {
                        suma_local[c][j] += BD[i][j]; 
                    }
                    count_local[c]++;
                }

                // Luego de que cada hilo calculara la suma local de datos se agrega esta a la
                // suma global de cada centroide
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

                // Actualiza los centroides, primero asegura que el centroide se actualice solo si
                // tiene vectores asignados a el, luego se actualiza el centroide dividiendo la suma
                // acumulada de sus vectores por la cantidad total de vectores asignados 
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

    double finT = omp_get_wtime();
    
    for (i = 0; i < SIZE; i++)
        printf("%d\n", asignaciones[i][0]);

    printf("Tiempo de ejecucion: %.4f segundos\n", finT - inicioT);
    
    // Liberacion de memoria
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

// Calcula la distancia euclidiana entre vectores con DIM dimensiones
float calcular_distancia(float *fila, float *centroide) {
    float distancia = 0.0;
    for (int i = 0; i < DIM; i++) {
        float diff = fila[i] - centroide[i];
        distancia += diff * diff;
    }
    return sqrt(distancia);
}
