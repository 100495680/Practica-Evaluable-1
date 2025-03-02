#include <stdio.h>
#include "claves.h"

int main() {
    int key = 10;
    char v1[MAX_STRING] = "Ejemplo";
    double v2[MAX_VECTOR] = {1.1, 2.2, 3.3};
    struct Coord v3 = {5, 6};

    printf("Insertando clave...\n");
    if (set_value(key, v1, 3, v2, v3) == 0) {
        printf("Clave insertada correctamente\n");
    } else {
        printf("Error al insertar clave\n");
    }

    char retrieved_v1[MAX_STRING];
    int N_value2;
    double retrieved_v2[MAX_VECTOR];
    struct Coord retrieved_v3;

    printf("Obteniendo clave...\n");
    if (get_value(key, retrieved_v1, &N_value2, retrieved_v2, &retrieved_v3) == 0) {
        printf("Clave obtenida correctamente: %s\n", retrieved_v1);
    } else {
        printf("Error al obtener clave\n");
    }

    return 0;
}
