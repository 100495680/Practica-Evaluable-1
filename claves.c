#include "claves.h"

typedef struct Tupla {
    int key;
    char value1[MAX_STRING];
    int N_value2;
    double V_value2[MAX_VECTOR];
    struct Coord value3;
    struct Tupla *next;
} Tupla;

Tupla *head = NULL;
pthread_mutex_t tupla_mutex = PTHREAD_MUTEX_INITIALIZER;

int destroy() {
    pthread_mutex_lock(&tupla_mutex);
    Tupla *current = head;
    while (current) {
        Tupla *temp = current;
        current = current->next;
        free(temp);
    }
    head = NULL;
    pthread_mutex_unlock(&tupla_mutex);
    return 0;
}

int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    pthread_mutex_lock(&tupla_mutex);
    if (N_value2 < 1 || N_value2 > MAX_VECTOR) {
        pthread_mutex_unlock(&tupla_mutex);
        return -1;
    };
    Tupla *current = head;
    Tupla *previous = NULL;
    while (current) { // Iterar hasta la última tupla
        previous = current;
        current = current->next;       
    }

    Tupla *new_tupla = (Tupla *) malloc(sizeof(Tupla));
    if (!new_tupla) {
        pthread_mutex_unlock(&tupla_mutex);
        return -1;
    }
  
    // Crear la nueva tupla
    new_tupla->key = key;
    strncpy(new_tupla->value1, value1, MAX_STRING - 1);
    new_tupla->N_value2 = N_value2;
    memcpy(new_tupla->V_value2, V_value2, N_value2 * sizeof(double));
    new_tupla->value3 = value3;
    new_tupla->next = NULL; 

    // Poner la nueva tupla en el final de la cola si hay cola y sino será la primera
    if (previous) 
        previous->next = new_tupla;
    else
        head = new_tupla;
    
    

    pthread_mutex_unlock(&tupla_mutex);
    return 0;
}

int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3) {
    pthread_mutex_lock(&tupla_mutex);

    Tupla *current = head;
    while (current) {
        if (current->key == key) {
            strncpy(value1, current->value1, MAX_STRING - 1);
            value1[MAX_STRING - 1] = '\0';  // Asegurar terminación nula
            *N_value2 = current->N_value2;
            memcpy(V_value2, current->V_value2, (*N_value2) * sizeof(double));
            *value3 = current->value3;
            pthread_mutex_unlock(&tupla_mutex);
            return 0;  // Éxito
        }
        current = current->next;
    }

    pthread_mutex_unlock(&tupla_mutex);
    return -1;  // Clave no encontrada
}

int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    if (N_value2 < 1 || N_value2 > MAX_VECTOR) return -1;
    pthread_mutex_lock(&tupla_mutex);
    Tupla *current = head;
    while (current) {
        if (current->key == key) { // Mirar si la clave esta en el programa
            strncpy(current->value1, value1, MAX_STRING - 1);
            current->N_value2 = N_value2;
            memcpy(current->V_value2, V_value2, N_value2 * sizeof(double));
            current->value3 = value3;
            pthread_mutex_unlock(&tupla_mutex);
            return 0;
        }
        current = current->next; // Convertir en NULL si no hay siguiente o iterar
    } // Si no se encuentra la clave, dará error.

    pthread_mutex_unlock(&tupla_mutex);
    return -1;
}

int delete_key(int key) {
    pthread_mutex_lock(&tupla_mutex);
    Tupla *current = head;
    Tupla *previous = NULL;
    while (current) {
        if (current->key == key) { // Mirar si la clave esta en el programa
            if (previous) 
                previous->next = current->next;
            free(current);
            head = NULL;
            pthread_mutex_unlock(&tupla_mutex);
            return 0;
        }
        previous = current;
        current = current->next; // Convertir en NULL si no hay siguiente o iterar
    } // Si no se encuentra la clave, dará error.

    pthread_mutex_unlock(&tupla_mutex);
    return -1;
}

int exist(int key) {
    pthread_mutex_lock(&tupla_mutex);
    Tupla *current = head;
    while (current) {
        if (current->key == key) { // Mirar si la clave esta en el programa
            pthread_mutex_unlock(&tupla_mutex);
            return 1;
        }
        current = current->next; // Convertir en NULL si no hay siguiente o iterar
    } // Si no se encuentra la clave, dará 0.

    pthread_mutex_unlock(&tupla_mutex);
    return 0;
}