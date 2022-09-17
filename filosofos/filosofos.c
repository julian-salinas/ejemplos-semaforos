#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#define CANTIDAD_FILOSOFOS  5
#define IZQUIERDA           (numero_filosofo - 1) % CANTIDAD_FILOSOFOS
#define DERECHA             (numero_filosofo + 1) % CANTIDAD_FILOSOFOS

typedef enum {
    PENSANDO,
    HAMBRIENTO,
    COMIENDO
} t_estado;

t_estado filosofos[CANTIDAD_FILOSOFOS];
sem_t sem_filosofos[CANTIDAD_FILOSOFOS];
pthread_mutex_t mutex_tenedores;

void filosofo(void* args);
void tomar_tenedores(int numero_filosofo);
void dejar_tenedores(int numero_filosofo);
void comprobar(int numero_filosofo);
bool podria_comer(int numero_filosofo);

int main(int argc, char** argv) {
    // Inicializar los semáforos de todos los filósofos en 0
    for (int i = 0; i < CANTIDAD_FILOSOFOS; i++) {
        filosofos[i] = PENSANDO;
        sem_init(&sem_filosofos[i], 0, 0);
    }

    // Inicializar semáforo mutex de tenedores (Al ser un mutex, se inicializa en 1)
    pthread_mutex_init(&mutex_tenedores, NULL);

    // Crear hilos de los filósofos para que se sienten en la mesa
    pthread_t filosofos[CANTIDAD_FILOSOFOS];
    for (int i = 0; i < CANTIDAD_FILOSOFOS; i++) {
        int* numero_filosofo = malloc(sizeof(int));
        *numero_filosofo = i;
        pthread_create(&filosofos[i], NULL, (void*) filosofo, numero_filosofo);
    }

    // Detener la ejecución del hilo main hasta que todos los filósofos terminen (spoiler: nunca)
    for (int i = 0; i < CANTIDAD_FILOSOFOS; i++) {
        pthread_join(filosofos[i], NULL);
    }

    return 0;
}

void filosofo(void* args) {
    int numero_filosofo = *((int*) args);
    printf("Filosofo %d se sienta a la mesa \n", numero_filosofo);

    while (1) {
        // Pensar
        printf("Filosofo %d esta pensando... \n", numero_filosofo);
        sleep(2); 
        printf("Filosofo %d tiene hambre \n", numero_filosofo);

        tomar_tenedores(numero_filosofo);

        // Comer
        printf("Filosofo %d esta comiendo... \n", numero_filosofo);
        sleep(2);
        printf("Filosofo %d termina de comer \n", numero_filosofo);

        dejar_tenedores(numero_filosofo);
    }
}

void tomar_tenedores(int numero_filosofo) {
    pthread_mutex_lock(&mutex_tenedores);

    filosofos[numero_filosofo] = HAMBRIENTO;
    comprobar(numero_filosofo);
    
    pthread_mutex_unlock(&mutex_tenedores);

    // Esperar a que le indiquen que puede tomar los tenedores
    sem_wait(&sem_filosofos[numero_filosofo]);
}

void dejar_tenedores(int numero_filosofo) {
    pthread_mutex_lock(&mutex_tenedores);

    filosofos[numero_filosofo] = PENSANDO;

    // Comprobar si los filósofos de los lados pueden comer
    comprobar((numero_filosofo - 1) % CANTIDAD_FILOSOFOS);
    comprobar((numero_filosofo + 1) % CANTIDAD_FILOSOFOS);

    pthread_mutex_unlock(&mutex_tenedores);
}

void comprobar(int numero_filosofo) {
    /**
     * Comprobar si un filósofo puede comer (tiene tenedores disponibles), en caso de que pueda, se le indica que puede tomar los tenedores
     */ 
    if (podria_comer(numero_filosofo)) {
        filosofos[numero_filosofo] = COMIENDO;
        sem_post(&sem_filosofos[numero_filosofo]);
    }
}

bool podria_comer(int numero_filosofo) {
    /**
     * Indica si el filósofo indicado podría proceder a comer, esto pasa si:
     * - El filósofo indicado está hambriendo 
     * - Los filósofos de su izquierda y derecha no están comiendo (no están comiendo -> no están usando tenedores)
     */
    return filosofos[numero_filosofo] == HAMBRIENTO 
           && filosofos[sentado_a_la_izquierda(numero_filosofo)] != COMIENDO 
           && filosofos[sentado_a_la_derecha(numero_filosofo)] != COMIENDO;
}

int sentado_a_la_izquierda(int numero_filosofo) {
    /** 
     *Devuelve el número del filósofo sentado a la derecha del filósofo indicado
     */
    return (numero_filosofo - 1) % CANTIDAD_FILOSOFOS;
}

int sentado_a_la_derecha(int numero_filosofo) {
    /** 
     *Devuelve el número del filósofo sentado a la derecha del filósofo indicado
     */
    return (numero_filosofo + 1) % CANTIDAD_FILOSOFOS;
}