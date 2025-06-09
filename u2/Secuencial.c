// Nombres Estudiantes: Kevin Olivares y Dominique Teillier
// Nombre Profesor: Ricardo Barrientos Rojel
// Fecha: 2025-04-28
// Ramo: Sistemas Distribuidos secc-2

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>

#define DIM 20  // Número de dimensiones de cada elemento
#define K 3     // Número de clústeres

float calcular_distancia(float *fila, float *centroide);

int main() {
    int i, j, SIZE;
    float **BD;

    double inicioT = omp_get_wtime(); // Comenzar tiempo de ejecución
    scanf("%d", &SIZE); // Leer cantidad de elementos de la base de datos

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

        // Asignar cada elemento al centroide más cercano
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

            asignaciones[i][0] = minIdx; // Número del clúster (1 a K)
            asignaciones[i][1] = i;      // Índice del elemento

            // Si la asignación cambió respecto a la iteración anterior
            if (asignaciones[i][0] != asignaciones_previas[i])
                cambio = 1;
        }

        // Recalcular los centroides como el promedio de los elementos asignados a cada uno
        for (int c = 1; c <= K; c++) {
            float suma[DIM] = {0}; // Suma acumulada para cada dimensión
            int count = 0;         // Número de elementos en el clúster

            for (i = 0; i < SIZE; i++) {
                if (asignaciones[i][0] == c) {
                    for (j = 0; j < DIM; j++) {
                        suma[j] += BD[asignaciones[i][1]][j];
                    }
                    count++;
                }
            }

            // Actualizar centroide solo si hay elementos en el clúster
            if (count > 0) {
                for (j = 0; j < DIM; j++) {
                    centroide[c - 1][j] = suma[j] / count;
                }
            }
        }

    } while (cambio); // Repetir mientras haya cambios en las asignaciones

    double finT = omp_get_wtime(); // Finalizar tiempo de ejecución

    // Imprimir el número del clúster asignado a cada elemento
    
    for (i = 0; i < SIZE; i++) {
        printf("%d\n", asignaciones[i][0]);
    }

    
    // Mostrar el tiempo de ejecución en segundos
    printf("Tiempo de ejecucion: %.4f segundos\n", finT - inicioT);

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

// Función que calcula la distancia Euclidiana entre un elemento (fila) y un centroide
float calcular_distancia(float *fila, float *centroide) {
    float distancia = 0.0;
    for (int i = 0; i < DIM; i++) {
        float diff = fila[i] - centroide[i];
        distancia += diff * diff;
    }
    return sqrt(distancia);  // Distancia Euclidiana
}
