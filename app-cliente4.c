#include <stdio.h>
#include "claves.h"

int main() {
    int key = 10;
    char v1[MAX_STRING] = "Ejemplo";
    double v2[MAX_VECTOR] = {1.1, 2.2, 3.3};
    struct Coord v3 = {5, 6};
    int response;


    // Prueba de exist sin existir
    printf("Destruimos la lista...\n");
    response = destroy();
    if (response == 0) {
        printf("Lista destruida\n");
    } else {
        if (response == -1) {
            printf("Error al insertar la clave\n");
        }
        if (response == -2) {
            printf("Error en el servidor\n");
        }
    }

    // Prueba de exist sin existir
    printf("Insertando clave...\n");
    response = set_value(key, v1, 3, v2, v3);
    if (response == 0) {
        printf("No existe la clave\n");
    } else {
        if (response == -1) {
            printf("Error al insertar la clave\n");
        }
        if (response == -2) {
            printf("Error en el servidor\n");
        }
    }

    return 0;
}
