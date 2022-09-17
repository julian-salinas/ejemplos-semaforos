/**
 * TODO: Pasar el enunciado a un readme copado
 * Un barbero atiende clientes a medida que van llegando, pero mientras no hay nadie, se duerme.
 * Cuando un cliente llega, le avisa al barbero que llegó. En ese momento, el barbero se
 * despierta y le corta el pelo, cuando no hay más clientes, el barbero vuelve a dormir.
 *
 * - Cuando el barbero atiende a un nuevo cliente, el asiento que ocupaba ese cliente se libera.
 * - Cuando llega un cliente, en caso de haber asientos, se queda esperando, en caso contrario, se va.
*/

#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

sem_t clientes_esperando;
pthread_mutex_t mutex_asientos_libres;

int asientos_libres;

void barbero(void* args) {
    while (1) {
        // Espera a que llegue un cliente
        sem_wait(&clientes_esperando);

        // Asegurar mutua exclusión sobre la variable asientos_libres para escribirla
        pthread_mutex_lock(&mutex_asientos_libres);
        
        asientos_libres++;
        printf("El barbero atiende a un nuevo cliente y se libera un asiento - Cantidad de asientos disponibles: %d \n", asientos_libres);
        
        pthread_mutex_unlock(&mutex_asientos_libres);

        // Cortar pelo
        printf("El barbero esta cortando el pelo... \n");
            sleep(6);
        printf("El barbero termina de cortar el pelo \n");
    }
}

void cliente(void* args) {
    while (1) {
        // Cada 3 segundos llega un nuevo cliente a la barbería
        sleep(3);
        printf("Llega un nuevo cliente \n");

        // Asegurar mutua exclusión sobre la variable asientos_libres para leerla y/o escribirla
        pthread_mutex_lock(&mutex_asientos_libres);
        
        if (!asientos_libres) {
            // Al no haber asientos libres, el cliente se va
            printf("No hay asientos libres, el cliente se va \n");

            // Liberar el mutex
            pthread_mutex_unlock(&mutex_asientos_libres);
            continue;
        } 

        // Ocupar un asiento
        asientos_libres--;
        printf("El cliente se sienta esperando a ser atendido - Cantidad de asientos disponibles: %d \n", asientos_libres);
        
        // Liberar el mutex
        pthread_mutex_unlock(&mutex_asientos_libres);
        
        // Avisar al barbero que llegó un cliente
        sem_post(&clientes_esperando);
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Decime la cantidad de asientos que tiene la barberia loco >:(");
        return 1;
    }

    // Definir cantidad de asientos que tiene la barbería
    asientos_libres = atoi(argv[1]);

    // Inicializar semáforo clientes_esperando en 0
    sem_init(&clientes_esperando, 0, 0); 

    // Inicializar semáforo mutex_asientos_libres (Al ser un mutex, se inicializa en 1)
    pthread_mutex_init(&mutex_asientos_libres, NULL);

    pthread_t hilo_barbero, hilo_cliente;
    pthread_create(&hilo_barbero, NULL, (void*) barbero, NULL);
    pthread_create(&hilo_cliente, NULL, (void*) cliente, NULL);

    // Frenar la ejecución del hilo main hasta que ambos hilos terminen
    pthread_join(hilo_barbero, NULL);
    pthread_join(hilo_cliente, NULL);

    return 0;
}
