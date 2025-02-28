#ifndef CLAVES_H
#define CLAVES_H

#include <stdio.h>

#define MAX_STRING 255
#define MAX_VECTOR 32

struct Coord {
    int x;
    int y;
};

int destroy();
int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3);
int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3);
int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3);
int delete_key(int key);
int exist(int key);

#endif