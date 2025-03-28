#ifndef CLAVES_H
#define CLAVES_H

#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_STRING 256
#define MAX_VECTOR 32

struct Coord {
    int x;
    int y;
};

struct peticion {
    int op;  // Tipo de operación (0=destroy, 1=set, 2=get, etc.)
    int key;
    char value1[MAX_STRING];
    int N_value2;
    double V_value2[MAX_VECTOR];
    struct Coord value3;
    char q_name[MAX_STRING];  // Cola del cliente para la respuesta
};

struct respuesta {
    int status; // 0=éxito, -1=error
    char value1[MAX_STRING];
    int N_value2;
    double V_value2[MAX_VECTOR];
    struct Coord value3;
};

// API para la aplicación cliente
int destroy(void);
int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3);
int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3);
int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3);
int delete_key(int key);
int exist(int key);

#endif
