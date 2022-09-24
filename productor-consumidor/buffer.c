#include <stdlib.h>
#include <stdio.h>

extern void* buffer[];
extern int tamanio_buffer;

void agregar_a_buffer(void* elemento) {
    for (int i = 0; i < tamanio_buffer; i++) { // Buscar el primer espacio vacio del buffer (NULL)
        if (buffer[i] == NULL) {
            buffer[i] = elemento; // Agrega el elemento al buffer
            return; // Salir de la función
        }
    }
    // Si llega hasta acá es porque no hay espacio en el buffer
    fprintf(stderr, "No se pudo agregar el elemento al buffer \n");
}

void* tomar_de_buffer(void) {
    char* elemento;

    for (int i = 0; i < tamanio_buffer; i++) { // Buscar el primer elemento del buffer que no sea NULL
        if (buffer[i] != NULL) {
            elemento = buffer[i]; // Tomar el elemento del buffer
            buffer[i] = NULL; // Liberar la posición del buffer para almacenar otro elemento en el futuro
            return elemento;
        }
    }
    // Si llega hasta acá es porque no hay elementos en el buffer
    fprintf(stderr, "No se pudo tomar el elemento del buffer \n");
}