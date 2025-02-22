/* Este archivo es como la base de datos que guarda en una Linked List las estructuras mandadas y tambien ejecuta
los comandos POSIX (set_value, get_value, modify_value, delete_key, exist, destroy) */

#include "claves.h"

// Definición de la estructura de la Linked List
struct LinkedList {
    int key;
    char value1[256];  // Cadena de 255 caracteres y el '\0'
    int N_value2;
    double V_value2[32];
    struct Coord value3;
    struct LinkedList* next;
};


