#include "claves.h"
#include <stdlib.h>
#include <string.h>

typedef struct Tupla {
    int key;
    char value1[MAX_STRING];
    int N_value2;
    double V_value2[MAX_VECTOR];
    struct Coord value3;
    struct Tupla *next;
} Tupla;

Tupla *head = NULL;

int destroy() {
    Tupla *current = head;
    while (current) {
        Tupla *temp = current;
        current = current->next;
        free(temp);
    }
    head = NULL;
    return 0;
}

int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    if (N_value2 < 1 || N_value2 > MAX_VECTOR) return -1;

    Tupla *new_tupla = (Tupla *)malloc(sizeof(Tupla));
    if (!new_tupla) return -1;

    new_tupla->key = key;
    strncpy(new_tupla->value1, value1, MAX_STRING);
    new_tupla->N_value2 = N_value2;
    memcpy(new_tupla->V_value2, V_value2, N_value2 * sizeof(double));
    new_tupla->value3 = value3;
    new_tupla->next = head;
    head = new_tupla;

    return 0;
}

int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3) {
    Tupla *current = head;
    while (current) {
        if (current->key == key) {
            strncpy(value1, current->value1, MAX_STRING);
            *N_value2 = current->N_value2;
            memcpy(V_value2, current->V_value2, (*N_value2) * sizeof(double));
            *value3 = current->value3;
            return 0;
        }
        current = current->next;
    }
    return -1;
}

// Implementar modify_value, delete_key y exist de forma similar.

int exist(int key) {
    Tupla *current = head;
    while (current) {
        if (current->key == key) return 1;
        current = current->next;
    }
    return 0;
}

int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    Tupla *current = head;
    while (current) {
        if (current->key == key) {
            strncpy(current->value1, value1, MAX_STRING);
            current->N_value2 = N_value2;
            memcpy(current->V_value2, V_value2, N_value2 * sizeof(double));
            current->value3 = value3;
            return 0;
        }
        current = current->next;
    }
    return -1;
}

int delete_key(int key) {
    Tupla **indirect = &head;
    while (*indirect) {
        if ((*indirect)->key == key) {
            Tupla *temp = *indirect;
            *indirect = (*indirect)->next;
            free(temp);
            return 0;
        }
        indirect = &((*indirect)->next);
    }
    return -1;
}