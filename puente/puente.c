#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>

pthread_mutex_t autos_esperando;

sem_t sem_puente, llego_auto_subida, llego_auto_bajada;

int cantidad_autos_esperando = 0;

int total_autos;

void autos_llegando(void* args);
void bajada(void* args);
void subida(void* args);

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Decime cuantos autos en total van a llegar por favor >:( \n");
        return 1;
    }

    // Definir cantidad de autos que van a llegar
    total_autos = atoi(argv[1]);

    // Inicializar semáforo mutex en 1
    pthread_mutex_init(&autos_esperando, NULL);

    // Semáforo binario para controlar el puente
    sem_init(&sem_puente, 0, 1);

    // Semáforos para controlar la llegada de autos para subir y bajar
    sem_init(&llego_auto_subida, 0, 0);
    sem_init(&llego_auto_bajada, 0, 0);

    // Crear los hilos
    pthread_t thread_autos_llegando, thread_autos_subida, thread_autos_bajada;
    pthread_create(&thread_autos_bajada, NULL, (void*) bajada, NULL);
    pthread_create(&thread_autos_subida, NULL, (void*) subida, NULL);
    pthread_create(&thread_autos_llegando, NULL, (void*) autos_llegando, NULL);

    pthread_join(thread_autos_llegando, NULL);
    pthread_join(thread_autos_bajada, NULL);
    pthread_join(thread_autos_subida, NULL);

    return 0;
}

void autos_llegando(void* args) {
    for(int i = 0; i < total_autos; i++) {
        pthread_mutex_lock(&autos_esperando);
        cantidad_autos_esperando++;
        printf("Autos esperando: %d \n", cantidad_autos_esperando);
        pthread_mutex_unlock(&autos_esperando);

        int r = rand() % 2; // Número aleatorio entre 0 y 1
        if (r == 0) {
            printf("Llega un auto por la subida \n");
            sem_post(&llego_auto_subida);
        } 
        else {
            printf("Llega un auto por la bajada \n");
            sem_post(&llego_auto_bajada);
        }
    }
}

void subida(void* args) {
    while (1) {
        sem_wait(&llego_auto_subida);

        sem_wait(&sem_puente);
        
        // Se sube el puente
        printf("Un auto accede a la subida del puente \n");
        sleep(3);
        printf("El auto termina de subir al puente \n");

        sem_post(&sem_puente);

        pthread_mutex_lock(&autos_esperando);
        cantidad_autos_esperando--;
        printf("Autos esperando: %d \n", cantidad_autos_esperando);
        pthread_mutex_unlock(&autos_esperando); 
    }
}

void bajada(void* args) {
    while (1) {
        sem_wait(&llego_auto_bajada);

        sem_wait(&sem_puente);
        
        // Se baja el puente
        printf("Un auto accede a la bajada \n");
        sleep(3);
        printf("El auto termina de bajar por el puente \n");
        
        sem_post(&sem_puente);
        
        pthread_mutex_lock(&autos_esperando);
        cantidad_autos_esperando--;
        printf("Autos esperando: %d \n", cantidad_autos_esperando);
        pthread_mutex_unlock(&autos_esperando);
    }
}