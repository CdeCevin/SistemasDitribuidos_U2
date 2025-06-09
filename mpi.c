
#include <stdio.h>
#include <mpi.h>
int main(int argc, char **argv) // Prototipo de la función main
{ 
int nproc; //Numero de procesos
int yo; // Mi dirección,: 0 <= yo < nproc - 1

int mensaje; // Mensaje a enviar
char mensaje2[] = "HEEEY MUY BUENAS A TODOS " ;
MPI_Status stat;

MPI_Init(&argc, &argv); // Initialize MPI
MPI_Comm_size(MPI_COMM_WORLD, &nproc); // Get number of processes
MPI_Comm_rank(MPI_COMM_WORLD, &yo); // Get rank of process



//Cuerpo del programa
printf("El numero de procesos es: %d \n", &nproc);
printf("Hola, yo es -> %d \n", yo);

if (yo == 0)
{
    mensaje = 111;
     
    MPI_Send(&mensaje, 1, MPI_INT, 3, 10, MPI_COMM_WORLD); // Send message to process 3
    printf("mensaje enviado");        
    MPI_Send(mensaje2, 1, MPI_CHAR, 2, 7, MPI_COMM_WORLD);
}
if(yo == 3)
{
    MPI_Recv(&mensaje, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat); // Receive message from process 0
    printf("Soy el nodo 3, el mensaje recibido es: %d del proceso: %d y con etiqueta: %d \n", mensaje, stat.MPI_SOURCE, stat.MPI_TAG);
}

if(yo == 2)
{
    MPI_Recv(mensaje2, 1, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD,&stat);
    printf("Soy el nodo 2, El mensaje recibido es: %s del proceso: %d y con etiqueta: %d \n", mensaje2, stat.MPI_SOURCE, stat.MPI_SOURCE);
}


//Cuerpo del programa

MPI_Finalize(); // Finalize MPI 

}





