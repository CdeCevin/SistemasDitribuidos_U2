#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <math.h>

#define DIM 20  // Número de dimensiones de cada elemento
#define K 3     // Número de clústeres
#define T 8     // Número de hilos para OpenMP

// Función que calcula la distancia Euclidiana entre un elemento (fila) y un centroide
float calcular_distancia(float *fila, float *centroide) {
    float distancia = 0.0;
    for (int i = 0; i < DIM; i++) {
        float diff = fila[i] - centroide[i];
        distancia += diff * diff;
    }
    return sqrt(distancia);  // Distancia Euclidiana
}

int main() {
    int i, j, SIZE;
    float **BD;

    scanf("%d", &SIZE); // Leer cantidad de elementos de la base de datos

    clock_t inicio = clock(); // Comenzar medición de tiempo de ejecución

    
    omp_set_dynamic(0);
    // Configura el número de hilos para la paralelización
    omp_set_num_threads(T);

    // Reservar memoria para la base de datos
    BD = (float **)malloc(sizeof(float *) * SIZE);
    for (i = 0; i < SIZE; i++)
        BD[i] = (float *)malloc(sizeof(float) * DIM);

    // Leer cada valor de los elementos
    for (i = 0; i < SIZE; i++)
        for (j = 0; j < DIM; j++)
            scanf("%f", &BD[i][j]);

    srand(time(NULL)); // Inicializar generador de números aleatorios

    // Reservar memoria para los centroides
    float **centroide = (float **)malloc(sizeof(float *) * K);
    for (i = 0; i < K; i++)
        centroide[i] = (float *)malloc(sizeof(float) * DIM);

    // Arreglo para evitar elegir elementos repetidos como centroides iniciales
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

        // Copiar el elemento como centroide
        for (j = 0; j < DIM; j++)
            centroide[i][j] = BD[idx][j];
    }

    // Arreglo que guarda la asignación de cada elemento al clúster
    // asignaciones[i][0] = número de clúster (1 a K), asignaciones[i][1] = índice del elemento
    int **asignaciones = (int **)malloc(sizeof(int *) * SIZE);
    for (i = 0; i < SIZE; i++)
        asignaciones[i] = (int *)malloc(sizeof(int) * 2);

    // Arreglo para verificar si hubo cambio en las asignaciones
    int *asignaciones_previas = (int *)malloc(sizeof(int) * SIZE);
    for (i = 0; i < SIZE; i++)
        asignaciones_previas[i] = -1;

    int cambio; // Bandera para saber si se realizaron cambios en la iteración

    // Algoritmo principal de K-means
    do {
        cambio = 0;
        // Guardar asignaciones previas
        for (i = 0; i < SIZE; i++)
            asignaciones_previas[i] = asignaciones[i][0];

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

        // Recalcular los centroides como el promedio de los elementos asignados a cada uno


    for (int c = 1; c <= K; c++) {
        float suma[DIM] = {0};
        int count = 0;

        #pragma omp parallel
        {
            int id = omp_get_thread_num();
            int num_threads = omp_get_num_threads();
            int elementos_por_hilo = (SIZE / num_threads);
            int inicio =  elementos_por_hilo * id;
            int fin;

            if(id==num_threads-1){
                fin = SIZE;
            }
            else{
                fin = inicio + elementos_por_hilo;
            }

            for (int i = inicio; i < fin; i++) {
                if (asignaciones[i][0] == c) {
                    for (int j = 0; j < DIM; j++) {
                        #pragma omp atomic
                        suma[j] += BD[asignaciones[i][1]][j];
                    }
                    #pragma omp atomic
                    count++;
                }
            }
        }

        if (count > 0) {
            for (int j = 0; j < DIM; j++) {
                centroide[c - 1][j] = suma[j] / count;
            }
        }
    }

    } while (cambio); // Repetir mientras haya cambios en las asignaciones

    clock_t fin = clock(); // Finalizar medición de tiempo
    
    // Imprimir el número del clúster asignado a cada elemento
    for (i = 0; i < SIZE; i++) {
        printf("%d\n", asignaciones[i][0]);
    }

    // Mostrar el tiempo de ejecución en segundos
    double tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC;
    printf("Tiempo de ejecucion: %.4f segundos\n", tiempo);

    // Liberar memoria dinámica utilizada
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
