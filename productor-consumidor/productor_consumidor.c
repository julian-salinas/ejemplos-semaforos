#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#define TAM_BUFFER 3

void* buffer[3];

void productor(void* args);
void consumidor(void* args);
char* producir_elemento(void);

pthread_mutex_t mutex_buffer;
sem_t contador_vacios;
sem_t contador_llenos;

void agregar_a_buffer(void* elemento) {
    for (int i = 0; i < TAM_BUFFER; i++) {
        if (buffer[i] == NULL) {
            buffer[i] = elemento;
            break;
        }
    }
}

void* tomar_de_buffer(void) {
    char* elemento;

    for (int i = 0; i < TAM_BUFFER; i++) {
        if (buffer[i] != NULL) {
            elemento = buffer[i];
            buffer[i] = NULL; // Libera el espacio del buffer
            return elemento;
        }
    }
}

void productor(void* args) {
    int id_productor = *((int*) args) + 1;
    free(args);

    while(1) {
        sem_wait(&contador_vacios); // Espera a que haya un espacio vacio
        pthread_mutex_lock(&mutex_buffer);
        char* elemento = producir_elemento();
        printf("Productor %d agrega elemento %c al buffer \n", id_productor, *elemento);
        agregar_a_buffer(elemento);
        pthread_mutex_unlock(&mutex_buffer);
        
        sem_post(&contador_llenos); // Avisa que hay un recurso producido (buffer lleno)
        
        sleep(3); // 😴 (Simula el tiempo que tarda en producir)
    }
}

char* producir_elemento(void) {
    char* elemento = malloc(sizeof(char));
    
    *elemento = "abcdefghijklmnopqrstuvwxyz"[rand() % 26];
    
    return elemento;
}

void consumidor(void* args) {
    int id_consumidor = *((int*) args) + 1;
    free(args);

    while(1) {
        sem_wait(&contador_llenos); // Espera a que haya un recurso producido (buffer lleno)

        pthread_mutex_lock(&mutex_buffer);
        char* elemento = (char*) tomar_de_buffer();
        printf("Consumidor %d consume %c\n", id_consumidor, *elemento);
        pthread_mutex_unlock(&mutex_buffer);

        sem_post(&contador_vacios); // Avisa que hay un espacio vacio (buffer vacio)
        
        sleep(2); // 😴 (Simula el tiempo que tarda en consumir)
        free(elemento); // Libera el recurso consumido
    }
}

int main(int argc, char** argv) {
    int cantidad_productores = 1, cantidad_consumidores = 3;

    if (argc == 3) {
        // Se reciben los parametros por consola para definir la cantidad de productores y consumidores
        cantidad_productores = atoi(argv[1]);
        cantidad_consumidores = atoi(argv[2]);
    }

    // Inicializar semáforos
    pthread_mutex_init(&mutex_buffer, NULL);
    sem_init(&contador_vacios, 0, TAM_BUFFER);
    sem_init(&contador_llenos, 0, 0);

    // Crear hilos de los productores
    pthread_t productores[cantidad_productores];
    for (int i = 0; i < cantidad_productores; i++) {
        int* id_productor = malloc(sizeof(int));
        *id_productor = i;
        pthread_create(&productores[i], NULL, (void*) productor, id_productor);
    }

    // Crear hilos de los consumidores
    pthread_t consumidores[cantidad_consumidores];
    for (int i = 0; i < cantidad_consumidores; i++) {
        int* id_consumidor = malloc(sizeof(int));
        *id_consumidor = i;
        pthread_create(&consumidores[i], NULL, (void*) consumidor, id_consumidor);
    }

    // Esperar a que terminen los hilos de los productores
    for (int i = 0; i < cantidad_productores; i++) {
        pthread_join(productores[i], NULL);
    }

    // Esperar a que terminen los hilos de los consumidores
    for (int i = 0; i < cantidad_consumidores; i++) {
        pthread_join(consumidores[i], NULL);
    }
}