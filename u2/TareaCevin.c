#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <windows.h>  // Para medir el tiempo en Windows

#define N 10000000  // Número de elementos en cada vector
#define T 8   // Número de hilos para paralelización

// Declaración de la función que genera un arreglo de números aleatorios
int *genera_num_aleatorios(int cantidad_elementos);

int main() {
    printf("Ejecutando Tarea2.c\n");
    int i, *vec_a, *vec_b, res_final = 0;
    LARGE_INTEGER t1, t2, freq;  // Variables para medir el tiempo
    double real_time;

    // Generación de dos vectores con números aleatorios
    vec_a = genera_num_aleatorios(N);

    vec_b = genera_num_aleatorios(N);
    int vec_c[] = {25,6,7,4,2};
    int vec_d[] = {25,6,7,4,2};

 // Mostrar los vectores
    /*printf("\nVector C: ");
    for (i = 0; i < N; i++) printf("%d ", vec_a[i]);
    printf("\nVector D: ");
    for (i = 0; i < N; i++) printf("%d ", vec_b[i]);
    printf("\n"); 
*/
    // Medición del tiempo
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t1);

    omp_set_num_threads(T);  // Configurar el número de hilos

    // Crear un arreglo para almacenar los resultados parciales de cada hilo
    int res_local[T];

    // Inicializar resultados locales
    for (i = 0; i < T; i++) {
        res_local[i] = 0;
    }

    // Región paralela donde se calcula el producto punto
    #pragma omp parallel
    
    {
        int id = omp_get_thread_num(); // ID del hilo
        int total_hilos = omp_get_num_threads(); // Total de hilos activos

        // Cada hilo procesa solo ciertas iteraciones del bucle
        for (int i = id; i < N; i += total_hilos) {
            //printf("Hilo %d multiplica: %d * %d = %d\n", id, vec_a[i], vec_b[i], vec_a[i] * vec_b[i]);
            res_local[id] += vec_a[i] * vec_b[i]; // Suma parcial del hilo
            //printf("Hilo %d suma parcial: %d\n", id, res_local[id]);
        }
    }

    // Sumar todos los resultados parciales
    for (i = 0; i < T; i++) {
        res_final += res_local[i];
    }

    // Finalizar la medición del tiempo
    QueryPerformanceCounter(&t2);
    real_time = (double)(t2.QuadPart - t1.QuadPart) / freq.QuadPart;

    // Imprimir el resultado final
    printf("\nResultado final del producto punto: %d", res_final);
    printf("\nTiempo Real = %f segundos\n", real_time);

    // Liberar memoria
    free(vec_a);
    free(vec_b);

    return 0;
}

// Función para generar un arreglo de números aleatorios
int *genera_num_aleatorios(int cantidad_elementos) {
    int i, *arr;
    arr = (int *)malloc(sizeof(int) * cantidad_elementos);
  

    for (i = 0; i < cantidad_elementos; i++) {
        arr[i] = i;
    }
    return arr;
}
