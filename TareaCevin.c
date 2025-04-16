#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <windows.h>  // Para medir el tiempo en Windows

#define N 10  // Número de elementos en cada vector
#define T 2   // Número de hilos para paralelización

// Declaración de la función que genera un arreglo de números aleatorios
int *genera_num_aleatorios(int cantidad_elementos);

int main() {
    printf("Ejecutando Tarea1.c\n");
    int i, *vec_a, *vec_b, res_final = 0;
    LARGE_INTEGER t1, t2, freq;  // Variables para medir el tiempo
    double real_time;

    // Generación de dos vectores con números aleatorios
    vec_a = genera_num_aleatorios(N);
    vec_b = genera_num_aleatorios(N);

    // Mostrar el contenido del vector A
    printf("Vector A: ");
    for (i = 0; i < N; i++) {
        printf("%d ", vec_a[i]);
    }
    printf("\n");

    // Mostrar el contenido del vector B
    printf("Vector B: ");
    for (i = 0; i < N; i++) {
        printf("%d ", vec_b[i]);
    }
    printf("\n");

    // Medición del tiempo: obtener la frecuencia del contador
    QueryPerformanceFrequency(&freq);
    // Inicia el contador de tiempo
    QueryPerformanceCounter(&t1);

    // Configura el número de hilos para la paralelización
    omp_set_num_threads(T);
    
    // Región paralela donde se calcula el producto punto
    #pragma omp parallel shared(vec_a, vec_b, res_final) firstprivate(i)
    {
        int res_local = 0; // Variable local para almacenar la suma parcial del hilo
        int id = omp_get_thread_num(); // Obtener el ID del hilo actual
        int total_hilos = omp_get_num_threads(); // Obtener el número total de hilos

        // Indicar que el hilo ha comenzado su trabajo
        printf("Hilo %d de %d ha comenzado su trabajo.\n", id, total_hilos);

        // Distribución del ciclo entre hilos sin espera implícita al final
        #pragma omp for nowait
        for (i = 0; i < N; i++) {
            // Cada hilo multiplica el elemento i de vec_a por el de vec_b y lo muestra
            printf("Hilo %d multiplica: %d * %d = %d\n", id, vec_a[i], vec_b[i], vec_a[i] * vec_b[i]);
            // Acumula el resultado de la multiplicación en la variable local

            res_local += vec_a[i] * vec_b[i];
            printf("Hilo %d suma parcial: %d\n", id, res_local);
        }

        // Mostrar el resultado parcial calculado por el hilo
        printf("Hilo %d ha terminado con una suma parcial de %d.\n", id, res_local);

        // Sección crítica para evitar condiciones de carrera al actualizar el resultado final
        #pragma omp critical
        {
            res_final += res_local;
            // Mostrar el resultado acumulado tras la suma del resultado parcial del hilo
            printf("Hilo %d ha sumado su resultado parcial. Resultado acumulado: %d\n", id, res_final);
        }
    }

    // Finalizar la medición del tiempo y calcular el tiempo real transcurrido
    QueryPerformanceCounter(&t2);
    real_time = (double)(t2.QuadPart - t1.QuadPart) / freq.QuadPart;

    // Imprimir el resultado final del producto punto y el tiempo de ejecución
    printf("\nResultado final del producto punto: %d", res_final);
    printf("\nTiempo Real = %f segundos\n", real_time);

    // Liberar la memoria asignada para los vectores
    free(vec_a);
    free(vec_b);

    return 0;
}

// Función para generar un arreglo de números aleatorios
int *genera_num_aleatorios(int cantidad_elementos) {
    int i, *arr;
    // Reservar memoria para el arreglo
    arr = (int *)malloc(sizeof(int) * cantidad_elementos);
   

    // Llenar el arreglo con números aleatorios entre 1 y 100
    for (i = 0; i < cantidad_elementos; i++) {
        arr[i] = (rand() % 100) + 1;
    }

    return arr;
}

